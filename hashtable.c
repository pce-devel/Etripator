#include "config.h"
#include "hashtable.h"
#include "random.h"

#define MAX_LEVEL 31
#define HASH_SEED 67295

/** \brief Murmur3 hash function
 * code from http://www.team5150.com/~andrew/noncryptohashzoo/Murmur3.html
 */
uint32_t Murmur3( const char *key, size_t len, uint32_t seed )
{
    #define mmix3(h,k) do { k *= m1; k = _rotl(k,r1); k *= m2; h *= 3; h ^= k; } while(0);
 
    const uint32_t m1 = 0x0acffe3d, m2 = 0x0e4ef5f3, m3 = 0xa729a897;
    const uint32_t r1 = 11, r2 = 18, r3 = 18;
    const uint8_t *tail;

    uint32_t h = len + seed, k = 0;
 
    const uint32_t *dwords = (const uint32_t *)key;
    while( len >= 4 ) {
        k = *dwords++;
        mmix3(h,k);
        len -= 4;
    }
 
    tail = (const uint8_t *)dwords;
    switch( len )
	{
        case 3: k ^= tail[2] << 16;
        case 2: k ^= tail[1] << 8;
        case 1: k ^= tail[0];
            mmix3(h,k);
    };
 
    h ^= h >> r2;
    h *= m3;
    h ^= h >> r3;
    return h;
}

/** \brief Compute node level by virtually flipping a coin */
static int RandomLevel(void)
{
	static 
	unsigned long u;
	int level;

#if defined(_MSC_VER)
	unsigned long index;
	unsigned char ret;
#endif

	while( !(u = CMWC4096()) )
	{}


#if defined(_MSC_VER)
	ret   = _BitScanForward(&index, u);
	level = (index & 0xffffffff);
#else
	level = __builtin_ctz(u) - 1;
#endif
	return level;
}

/** \brief Create a node for current key,value. */
static SkipListNode* SLCreateNode(int level, const char* key, size_t keyLen, uintptr_t value)
{
	size_t nodeSize = sizeof(SkipListNode) + (level+1) * sizeof(SkipListNode) + keyLen;
	SkipListNode *node = (SkipListNode*)malloc(nodeSize);
	
	if(node == NULL)
	{
		return NULL;
	}

	node->level = level;
	node->value = value;
	node->next  = (SkipListNode**)(node + 1);
	node->key   = (char*)(node->next + (level+1));
	node->hash  = 0;

	memset(node->next, 0, (level+1) * sizeof(SkipListNode));
	memcpy(node->key, key, keyLen);

	return node;
}

/**
 * \brief Create/Initialize skip list.
 *
 * \param list  Skip list to initialize.
 * \return 0 if an error occured, 1 on success.
 */
int SLCreate(SkipList* list)
{
	list->maxLevel = MAX_LEVEL;
	list->level    = 0;

	list->head = SLCreateNode(list->maxLevel, " ", 2, 0);
	if(list->head == NULL)
	{
		return 0;
	}
	return 1;
}

/**
 * \brief Release memory used by the current skip list.
 *
 * \param list  Skip list to destroy.
 */
void SLDestroy(SkipList* list)
{
	SkipListNode *current, *next;

	for(current=list->head; current!=NULL;)
	{
		next = current->next[0];
		free(current);
		current = next;
	}

	list->head = NULL;
	list->maxLevel = 0;
	list->level    = 0;
}

/**
 * \brief Add element to the skip list.
 *
 * \param list   Skip list where the new element will be stored.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \param value  Value.
 * \return 0 if an error occured, 1 on success.
 */
int SLAdd(SkipList* list, const char* key, size_t keyLen, uintptr_t value)
{
	SkipListNode *update[MAX_LEVEL];
	SkipListNode *current = list->head, *node;
	uint32_t hash;
	int i, level;

	memset(update, 0, MAX_LEVEL * sizeof(SkipListNode*));

	hash = Murmur3(key, keyLen, HASH_SEED);

	for(i=current->level-1; i>=0; i--)
	{
		while((current->next[i] != NULL) && (current->next[i]->hash < hash))
		{
			current = current->next[i];
		}
		update[i] = current;
	}

	if((current->next[0] != NULL) && (current->next[0]->hash == hash))
	{
		return 0;
	}

	level = RandomLevel();
	if(level > list->level)
	{
		update[list->level++] = list->head;
		level = list->level;
	}

	node  = SLCreateNode(level, key, keyLen, value);
	if(node == NULL)
	{
		return 0;
	}
	node->hash = hash;
	
	for(i=0; i<=level; i++)
	{
		node->next[i] = update[i]->next[i];
		update[i]->next[i] = node;
	}

	return 1;
}

/**
 * \brief Find the value associated to the key.
 *
 * \param list   Skip list where the value is supposed to be stored.
 * \param result Retrieved value.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \return 0 if the element was not found, 1 otherwise.
 */
int SLFind(const SkipList* list, uintptr_t* result, const char* key, size_t keyLen)
{
	SkipListNode *current;
	int i;
	uint32_t hash;

	hash = Murmur3(key, keyLen, HASH_SEED);

	current = list->head;
	for(i=list->level-1; i>=0; i--)
	{
		while((current->next[i] != NULL) && (current->next[i]->hash < hash))
		{
			current = current->next[i];
		}
	}

	current = current->next[0];
	if((current != NULL) && (current->hash == hash))
	{
		*result = current->value;
		return 1;
	}

	*result = 0;
	return 0;
}