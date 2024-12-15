#include "editFile.h"

static void updateCursor(Node* current)
{
	int x = current->character == NEWLINE ? current->x = 0 : current->x + 1;
	int y = current->character == NEWLINE ?
		current->y + 1 : 
		current->character == ESCAPE ? current->y - 1 : 
		current->y;

	setCursor(x, y);
}

static void writeText(Node *head)
{
	clear();
	for (Node* node = head; node; node = node->next)
		write(node->character);
}

static inline void moveLeft(Node** current)
{
	if ((*current)->prev)
		(*current) = (*current)->prev;
}

static void moveRight(Node** current)
{
	if ((*current)->next)
		(*current) = (*current)->next;
}

static void moveUp(Node** current)
{
	for (Node* node = *current; node->prev; node = node->prev) {
		if (node->y != (*current)->y && node->x == (*current)->x) {
			(*current) = node;
			break;
		}
	}
}

static inline void moveDown(Node** current)
{
	for (Node* node = *current; node->prev; node = node->next);
}

static bool moveCursor(int keyDirection, Node** current)
{
	switch (keyDirection)
	{
	case VK_LEFT:
		moveLeft(current);
		break;
	case VK_RIGHT:
		moveRight(current);
		break;
	case VK_UP:
		moveUp(current);
		break;
	case VK_DOWN:
		moveDown(current);
		break;
	default:
		return false;
	}
	return true;
}

static bool editText(
	NodeData* nodeData, 
	Node **current, 
	Node* head,
	wchar_t character
)
{
	Node *node = *current ? nodeData->pool[getFreeNode(nodeData)] : head;
	if (!node)
		return false;

	character = cleanCarriage(character);
	node->character = character;

	if (!getCursor(&node->x, &node->y)) {
		node->inUse = false;
		return false;
	}

	if (node->character == '\n' && (*current)) {
		node->x = (*current)->prev->x + 1;
		node->x = (*current)->prev->y;
	}

	node->character = character;
	character != BACKSPACE ? add(current, node) : erase(current, node);
	return true;
}

int edit(void) {
	bool isVk = false, keyProcessResult = false;
	wchar_t character = NOKEY;
	NodeData nodeData;
	Node *current = NULL, *head = NULL;
	
	if (!createPool(&nodeData, 2) || !init())
		return -1;

	head = nodeData.pool[getFreeNode(&nodeData)];

	while (character != ESCAPE) {
		if (!read(&character, &isVk))
			continue;

		keyProcessResult = isVk ?
			moveCursor((int)character, &current) :
			editText(&nodeData, &current, head, character);
		
		if (!keyProcessResult)
			continue;

		writeText(head);
		updateCursor(current);
	}

	deletePool(&nodeData);

	return 0;
}