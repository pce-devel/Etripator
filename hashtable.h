#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "config.h"

/** \brief Skip list node */
typedef struct _SkipListNode
{
	uint32_t  hash;  /**< Key hash value. */
	char      *key;  /**< String key.     */
	uintptr_t value; /**< Value.          */
	int       level; /**< Node level.     */

	struct _SkipListNode **next; /**< Array of "level" pointer to next nodes. */
} SkipListNode;

/* Skip list */
typedef struct
{
	int maxLevel;       /**< Maximal level. */
	int level;          /**< Current level. */
	SkipListNode* head; /**< List head.     */
} SkipList;

/**
 * \brief Create/Initialize skip list.
 *
 * \param list  Skip list to initialize.
 * \return 0 if an error occured, 1 on success.
 */
int SLCreate(SkipList* list);

/**
 * \brief Release memory used by the current skip list.
 *
 * \param list  Skip list to destroy.
 */
void SLDestroy(SkipList* list);

/**
 * \brief Add element to the skip list.
 *
 * \param list   Skip list where the new element will be stored.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \param value  Value.
 * \return 0 if an error occured, 1 on success.
 */
int SLAdd(SkipList* list, const char* key, size_t keyLen, uintptr_t value);

/**
 * \brief Find the value associated to the key.
 *
 * \param list   Skip list where the value is supposed to be stored.
 * \param result Retrieved value.
 * \param key    Hash key.
 * \param keyLen Length of the hash key.
 * \return 0 if the element was not found, 1 otherwise.
 */
int SLFind(const SkipList* list, uintptr_t *result, const char* key, size_t keyLen);


#endif // HASHTABLE_H