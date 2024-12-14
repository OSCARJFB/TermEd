#include "list.h"

static void setNodes(Node** nodes, size_t size, size_t start)
{
	for (size_t i = start; i < size; ++i) {
		nodes[i]->inUse = false;
		nodes[i]->x = nodes[i]->y = 0;
		nodes[i]->character = '\0';
		nodes[i]->next = nodes[i]->prev = NULL;
	}
}

static bool allocateNodesInPool(size_t start, size_t poolSize, Node **pool)
{
	for (size_t i = start; i < poolSize; ++i) {
		pool[i] = malloc(sizeof(Node));
		if (!pool[i]) {
			for (size_t n = i; n > 0; --n) {
				free(pool[n - 1]);
				pool[n - 1] = NULL;
			}
			free(pool);
			pool = NULL;
			return false;
		}
	}
	return true;
}

void deletePool(NodeData *nodeData)
{
	for (size_t i = 0; i < nodeData->poolSize; ++i) {
		free(nodeData->pool[i]);
		nodeData->pool[i] = NULL;
	}
	free(nodeData->pool);
	nodeData->pool = NULL;
}

bool createPool(NodeData* nodeData, size_t poolSize)
{
	nodeData->pool = malloc(sizeof(Node*) * poolSize);
	nodeData->poolSize = poolSize;
	if (!nodeData->pool || !allocateNodesInPool(0, poolSize, nodeData->pool))
		return false;

	setNodes(nodeData->pool, poolSize, 0);
	return true;
}

static bool resizePool(NodeData *nodeData)
{
	size_t newSize = nodeData->poolSize * 2;
	Node** tempNode = realloc(nodeData->pool, sizeof(Node*) * newSize);
	if (!tempNode || !allocateNodesInPool(nodeData->poolSize, newSize, tempNode))
		return false;

	setNodes(tempNode, newSize, nodeData->poolSize);
	nodeData->poolSize = newSize;
	nodeData->pool = tempNode;
	return true;
}

size_t getFreeNode(NodeData* nodeData)
{
	const size_t size = nodeData->poolSize;

	for (size_t i = 0; i < size; ++i) {
		if (!nodeData->pool[i]->inUse) {
			nodeData->pool[i]->inUse = true;
			return i;
		}
	}

	if (!resizePool(nodeData))
		return -1;
	nodeData->pool[size]->inUse = true;
	return size;
}

static void linkTail(Node** tail, Node* node)
{
	(*tail)->next = node;
	node->prev = *tail;
}

static void linkMiddle(Node** current, Node* node)
{
	Node* next = (*current)->next;

	(*current)->next = node;
	node->prev = *current;

	node->next = next;
	next->prev = node;

	*current = node;
}

static void linkHead(Node** head, Node* node)
{
	wchar_t character = node->character;
	Node* next = (*head)->next;

	node->character = (*head)->character;
	(*head)->character = character;

	node->next = next;
	next->prev = node;

	(*head)->next = node;
	node->prev = (*head);
}

void add(Node** current, Node* node)
{
	if (!*current)
		(*current) = node;
	else if (!(*current)->next)
		linkTail(current, node);
	else if ((*current)->next && (*current)->prev)
		linkMiddle(current, node);
	else if ((*current)->next && !(*current)->prev)
		linkHead(current, node);
	
	(*current) = node;
}

static void delinkTail(Node **tail, Node *new_current)
{
	(*tail)->next = NULL;
	(*tail)->prev = NULL;
	(*tail)->inUse = false;
	(*tail)->character = '\0';
	new_current->next = NULL;
}

static void delinkMiddle(Node** current, Node* node)
{

}

static void delinkHead(Node** head, Node* node)
{

}

void erase(Node** current, Node* node)
{
	Node* new_current = (*current)->prev;
	if (*current && !(*current)->next && (*current)->prev != NULL)
		delinkTail(current, new_current);
	if(new_current)
		(*current) = new_current;
}

