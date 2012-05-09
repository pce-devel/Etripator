/*
    This file is part of Etripator,
    copyright (c) 2009--2012 Vincent Cruz.

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
#include "rbtree.h"

/* Rotate a subtree */
static Node* Rotate(Node *x, unsigned char direction, Node **root)
{
	Node *y = x->child[!direction];

	x->child[!direction] = y->child[direction];
	
	if(y->child[direction] != NULL)
	{
		y->child[direction]->parent = x;
	}
	
    y->parent = x->parent;
    
    if(x == *root)
    {
    	*root = y;
    }
    else
    {
    	if(x->parent->child[direction] == x)
    		x->parent->child[direction] = y;
    	else
        	x->parent->child[!direction] = y;
    }

    y->child[direction] = x;
    x->parent			= y;
    /*
    y->color = BLACK;
    x->color = RED;
    */
    return y;
}

/* Insert data in the tree */
Node* Insert(Node **root, unsigned int value)
{
	Node *grandparent, *parent, *iterator;
	unsigned char direction;
	
	if(*root == NULL)
	{
		*root = (Node*)malloc(sizeof(Node));	
		(*root)->parent = (*root)->child[0] = (*root)->child[1] = NULL;
		(*root)->color = BLACK;
		(*root)->value = value;
		
		return *root;
	}
	
	parent    = NULL;
	iterator  = *root;
	direction = 0;	
	
	while(iterator != NULL)
	{
		if(value == iterator->value)
			return iterator;
			
		direction = (value > iterator->value) ? 1 : 0;
		
		parent   = iterator;
		iterator = iterator->child[direction];
	}
	
	parent->child[direction] = iterator = (Node*)malloc(sizeof(Node));
	iterator->parent = parent;
	iterator->child[0] = iterator->child[1] = NULL;
	iterator->color = RED;
	iterator->value = value;
	
	while((iterator != *root) && (parent->color == RED))
	{
		Node* uncle;
		
		grandparent = parent->parent;
		
		if(parent == grandparent->child[LEFT])
			direction = LEFT;
		else
			direction = RIGHT;
			
		uncle = grandparent->child[!direction];
		
		if((uncle != NULL) && (uncle->color == RED))
		{
			parent->color = BLACK;
			uncle->color  = BLACK;
			
			parent->parent->color = RED;
			iterator    = parent->parent;
			parent      = iterator->parent;
		}
		else
		{
			if(iterator == parent->child[!direction])
			{
				iterator = parent;
				Rotate(iterator, direction, root);
				parent      = iterator->parent;
			}
			
			parent->color = BLACK;
			grandparent->color = RED;
			Rotate(grandparent, !direction, root);
		}
	}
	
	(*root)->color = BLACK;
	
	return iterator;
}

/* Check tree */
int Check(Node* node)
{
	if(node == NULL)
		return 1;
	else
	{
		int leftCount, rightCount;
		Node* left  = node->child[LEFT];
		Node* right = node->child[RIGHT];
		
		if(node && (node->color == RED))
		{
			if((left  && (left->color  == RED)) ||
			   (right && (right->color == RED)))
			{
				printf("RED VIOLATION!\n");
				return 0;
			}
		}
		
		leftCount  = Check(left);
		rightCount = Check(right);
		
		if((left  && (left->value  >= node->value)) ||
		   (right && (right->value <= node->value)))
		{
			printf("BINARY TREE VIOLATION\n");
			return 0;
		}
		
		if(leftCount && rightCount && (leftCount != rightCount))
		{
			printf("BLACK VIOLATION\n");
			return 0;
		}
		
		if(leftCount && rightCount)
		{
			if(node->color == RED)
				return leftCount;
			else
				return leftCount + 1;
		}
		else
			return 0;
	}
}

/* Destroy tree */
void Destroy(Node** root)
{
	/* With a pool allocator, tree destruction will be faster ... */
	/* Stupid tree destruction ahead ! */
	Node *node, *tmp;

	node = *root;
	while(node != NULL)
	{
		if(node->child[0] == NULL)
		{
			if(node->child[1] != NULL)
			{
				node = node->child[1];
			}
			else
			{
				tmp = node->parent;
				if(tmp != NULL)
				{
					tmp->child[(tmp->child[1] == node) ? 1 : 0] = NULL;
				}
				free(node);
				node = tmp;
			}
		}
		else
		{
			node = node->child[0];
		}
	}
	*root = NULL;
}


/* Search for value in the tree */
Node* Search(Node* root, unsigned int value)
{
	unsigned char direction = 0;
	Node* iterator;

	iterator = root;
	while(iterator != NULL)
	{
		if(iterator->value == value)
			break;

		direction = (value > iterator->value) ? 1 : 0;
		iterator = iterator->child[direction];
	}

	return iterator;
}

/* Delete data from the tree */
void Delete(Node** root, Node* node)
{
	Node *iterator, *child, *parent, *sister;
	unsigned char direction, color;
	
	iterator = node;

	if((iterator->child[LEFT]  != NULL) &&
	   (iterator->child[RIGHT] != NULL))
	{
		Node* left;
		iterator = iterator->child[RIGHT];
		while((left = iterator->child[LEFT]) != NULL)
			iterator = left;

		child  = iterator->child[RIGHT];
		parent = iterator->parent;
		color  = iterator->color;

		if(child != NULL)
			child->parent = parent;

		if(parent)
		{
			if(parent->child[LEFT] == iterator)
				parent->child[LEFT] = child;
			else
				parent->child[RIGHT] = child;
		}
		else
			*root = child;

		if(iterator->parent == node)
			parent = iterator;

		iterator->child[0] = node->child[0];
		iterator->child[1] = node->child[1];
		iterator->parent   = node->parent;
		iterator->color    = node->color;

		if(node->parent != NULL)
		{
			if(node->parent->child[LEFT] == node)
				node->parent->child[LEFT] = iterator;
			else
				node->parent->child[RIGHT] = iterator;
		}
		else
			*root = iterator;

		node->child[LEFT]->parent = iterator;
		if(node->child[RIGHT] != NULL)
			node->child[RIGHT]->parent = iterator;
	}
	else
	{
		if(iterator->child[LEFT] == NULL)
			child = iterator->child[RIGHT];
		else 
			child = iterator->child[LEFT];

		parent = iterator->parent;
		color  = iterator->color;

		if(child)
			child->parent = parent;
		if(parent)
		{
			if(parent->child[LEFT] == iterator)
				parent->child[LEFT] = child;
			else
				parent->child[RIGHT] = child;
		}
		else
			*root = child;

		if((child == NULL) && (parent == NULL))
		{
			free(node);
			return;
		}
	}

	/* Fix tree */
	if(color == BLACK)
	{
		int up = 0;	
		iterator = child;
		while((iterator == NULL) || ((iterator->color == BLACK) && (iterator != *root)))
		{
			++up;
			direction = (iterator == parent->child[RIGHT]) ? RIGHT : LEFT;
			sister = parent->child[!direction];

			if(sister->color == RED)
			{
				sister->color = BLACK;
				parent->color = RED;

				Rotate(parent, direction, root);
				sister = parent->child[!direction];	
			}

			if(((sister->child[ direction] == NULL) ||
			    (sister->child[ direction]->color == BLACK)) && 
			   ((sister->child[!direction] == NULL) ||
			    (sister->child[!direction]->color == BLACK)))
			{
				sister->color = RED;
				iterator = parent;
				parent = iterator->parent;
			}
			else
			{
				if((sister->child[!direction] == NULL) ||
				   (sister->child[!direction]->color == BLACK))
				{
					if(sister->child[direction] != NULL)
						sister->child[direction]->color = BLACK;
					sister->color = RED;
					Rotate(sister, !direction, root);
					sister = parent->child[!direction];
				}

				sister->color = parent->color;
				parent->color = BLACK;
				if(sister->child[!direction])
					sister->child[!direction]->color = BLACK;
				Rotate(parent, direction , root);
				iterator = *root;
				break;
			}
		}
		if(iterator)
			iterator->color = BLACK;
	}

	free(node);
}

#ifdef RB_TREE_TEST

#define NB_NODE 1024

int main()
{
	Node *root, *node;
	int i;
	struct timeval start, end;
	double msec;
	
	uint32_t value;
	
	root = NULL;

	printf("Insert\n");
	gettimeofday(&start, NULL);
	for(i=0; i<NB_NODE; ++i)
	{
		value = 1 + (unsigned int) ((256.0 * rand()) / (RAND_MAX + 1.0));
		Insert(&root, value);
		if(Check(root) == 0)		
		{
			Destroy(&root);
			return 1;
		}
	}
	gettimeofday(&end, NULL);
	
	msec = ((double)end.tv_sec * 1000.0) + ((double)end.tv_usec / 1000.0);
	msec -= ((double)start.tv_sec * 1000.0) + ((double)start.tv_usec / 1000.0);

	printf("total : %f\n", msec);

	printf("Delete\n");	
	gettimeofday(&start, NULL);
	for(i=0; i<NB_NODE; ++i)
	{
		value = 1 + (unsigned int) ((256.0 * rand()) / (RAND_MAX + 1.0));
		node = Search(root, value);
		if(node)
		{
			Delete(&root, node);		
			if(Check(root) == 0)
			{
				Destroy(&root);
				return 1;
			}
		}
	}
	gettimeofday(&end, NULL);
	
	msec = ((double)end.tv_sec * 1000.0) + ((double)end.tv_usec / 1000.0);
	msec -= ((double)start.tv_sec * 1000.0) + ((double)start.tv_usec / 1000.0);

	printf("total : %f\n", msec);
	
	Destroy(&root);
	return 0;
}
#endif /* RB_TREE_TEST */
