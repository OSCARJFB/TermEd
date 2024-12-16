#ifndef EDIT_FILE_H
#define EDIT_FILE_H

#include "list.h"
#include "terminal.h"
#include "keys.h"

#define onTab(currentPos) currentPos + 8;
#define cleanCarriage(character) character == CARRIAGE ? NEWLINE : character;

int edit(void);

#endif // !EDIT_FILE_H
