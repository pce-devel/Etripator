/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

    Etripator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Etripator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include "config.h"

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

#endif // RED_BLACK_TREE_H
