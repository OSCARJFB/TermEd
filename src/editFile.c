#include "editFile.h"

static void updateCursor(Node* current)
{
	int x = current->character == NEWLINE ? 0 : current->x + 1;
	int y = current->character == NEWLINE ?
		current->y + 1 : 
		current->character == ESCAPE ? current->y - 1 : 
		current->y;

	if (current->character == TAB)
		x = onTab(current->x);

	setCursor(x, y);
}

static void writeText(Node *head)
{
	clear();
	for (Node* node = head; node; node = node->next) {
		if (node->character == TAB)
			for (int i = 0; i < 8; ++i)
				write(SPACE);
		else 
			write(node->character);
	}
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
	Node* newNode = NULL;

	for (Node* node = *current; node && node->prev; node = node->prev) {
		if (node->y < (*current)->y) {
			newNode = node;
			if (node->prev) {
				for (node = node->prev; node->prev && node->character != '\n'; node = node->prev) {
					if (node->x == (*current)->x) {
						newNode = node;
						break;
					}
				}
			}
			(*current) = newNode;
			break;
		}
	}
}

static inline void moveDown(Node** current)
{
	Node* newNode = NULL;

	for (Node* node = *current; node && node->next; node = node->next) {
		if (node->y > (*current)->y) {
			newNode = node;
			if (node->next) {
				for (node = node->next; node->next && node->character != '\n'; node = node->next) {
					if (node->x == (*current)->x) {
						newNode = node;
						break;
					}
				}
			}
			(*current) = newNode;
			break;
		}
	}
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

	setCursor((*current)->x, (*current)->y);
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

	node->character = character;
	character != BACKSPACE ? add(current, node) : erase(current, node);
	return true;
}

int edit(void) {
	bool isVk = false;
	wchar_t character = NOKEY;
	NodeData nodeData;
	Node *current = NULL, *head = NULL;
	
	if (!createPool(&nodeData, 2) || !init())
		return -1;
	head = nodeData.pool[getFreeNode(&nodeData)];

	while (character != ESCAPE) {
		if (!read(&character, &isVk))
			continue;
		if (isVk && moveCursor((int)character, &current))
			continue;
		if (!editText(&nodeData, &current, head, character))
			continue;
		writeText(head);
		updateCursor(current);
	}

	deletePool(&nodeData);

	return 0;
}