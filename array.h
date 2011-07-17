#ifndef ARRAY_H
#define ARRAY_H

// Todo: example

/**
 * \brief Array errors
 */
typedef enum
{
	ARRAY_OK = 0,          /*< Success. */
	ARRAY_MEMALLOC_ERROR,  /*< Unable to allocate memory. */
	ARRAY_EMPTY,           /*< The array is empty. */
	ARRAY_UNKNOWN_ERROR    /*< Unknown error. \note: must be the last one. */
} ARRAY_ERR;

/**
 * \brief Declares an array structure along create/destroy/push/destroy functions.
 */
#define ARRAY_DECLARE(type) \
typedef struct \
{ \
type *buffer; \
size_t capacity; \
size_t size; \
} type##_array; \
ARRAY_ERR type##_array_create(type##_array* container, size_t size); \
void type##_array_destroy(type##_array* container); \
ARRAY_ERR type##_array_grow(type##_array *container, size_t size); \
ARRAY_ERR type##_array_push(type##_array *container, type* element); \
ARRAY_ERR type##_array_pop(type##_array *container); \
void type##_array_reset(type##_array *container); \


/**
 * \brief Helper macro for declaring an array variable.
 */
#define ARRAY(type) type##_array
/**
 * \brief Helper macro for calling an array function.
 */
#define ARRAY_FUNC(type, func) type##_array_##func

/**
 * \brief Array functions definition.
 */
#define ARRAY_DEFINE(type) \
ARRAY_ERR type##_array_create(type##_array* container, size_t size) \
{ \
	container->buffer = NULL; \
	container->capacity = 0; \
	container->size = 0; \
	return type##_array_grow(container, size); \
} \
\
void type##_array_destroy(type##_array* container) \
{ \
	if(container->buffer != NULL) \
	{ \
		free(container->buffer); \
		container->buffer = NULL; \
	} \
	container->capacity = container->size = 0; \
} \
\
ARRAY_ERR type##_array_grow(type##_array *container, size_t size) \
{ \
	size_t capacity = container->capacity + size; \
	type* tmp; \
	tmp = (type*)realloc(container->buffer, capacity*sizeof(type)); \
	if(tmp == NULL) \
	{ \
		return ARRAY_MEMALLOC_ERROR; \
	} \
	container->capacity = capacity; \
	container->buffer = tmp; \
	return ARRAY_OK; \
} \
\
ARRAY_ERR type##_array_push(type##_array *container, type* element) \
{ \
	container->size++; \
	if(container->size >= container->capacity) \
	{ \
		ARRAY_ERR err = type ## _array_grow(container, container->capacity); \
		if( err != ARRAY_OK ) \
		{ \
			return err; \
		} \
	} \
	memcpy(container->buffer+container->size-1, element, sizeof(type)); \
	return ARRAY_OK; \
} \
ARRAY_ERR type##_array_pop(type##_array *container) \
{ \
	if(!container->size) \
	{ \
		return ARRAY_EMPTY; \
	} \
	--container->size; \
	return ARRAY_OK; \
} \
void type##_array_reset(type##_array *container) \
{ \
	container->size = 0; \
} \

#endif // ARRAY_H