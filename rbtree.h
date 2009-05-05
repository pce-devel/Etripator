#ifndef _RED_BLACK_TREE_H_
#define _RED_BLACK_TREE_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define LEFT  0
#define RIGHT 1

#define BLACK 0
#define RED 1

/* Red Black tree node */
struct Node_
{
	uint8_t       color;	/* Node color (Red or Black) */
	struct Node_ *parent;	/* Parent                    */
	struct Node_ *child[2];	/* Left and right child      */
	
	unsigned int value;	/* label offset              */
};

typedef struct Node_ Node;

/* Insert data in the tree */
Node* Insert(Node **, unsigned int);

/* Search for value in the tree */
Node* Search(Node*, unsigned int);

/* Delete data from the tree */
void Delete(Node** root, Node* node);

/* Destroy tree */
void Destroy(Node** root);

/* Check tree */
int Check(Node* node);

#endif /* _RED_BLACK_TREE_H_ */
