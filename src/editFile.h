#ifndef EDIT_FILE_H
#define EDIT_FILE_H

#include "list.h"
#include "terminal.h"
#include "keys.h"

#define cleanCarriage(character) character == CARRIAGE ? NEWLINE : character;

int edit(void);

#endif // !EDIT_FILE_H
