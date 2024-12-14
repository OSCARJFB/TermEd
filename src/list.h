#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Node
{
	bool inUse;
	int x, y;
	wchar_t character;
	struct Node* next, * prev;
} Node;

typedef struct NodeData
{
	Node **pool;
	size_t poolSize;
} NodeData;

bool createPool(NodeData *nodeData, size_t poolSize);
void deletePool(NodeData* nodeData);
size_t getFreeNode(NodeData* nodeData);
void add(Node** current, Node* node);
void erase(Node** current, Node* node);

#endif // !LIST_H
