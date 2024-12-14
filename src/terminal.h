#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <Windows.h>

bool init(void);
bool clear(void);
bool write(wchar_t cWrite);
bool read(wchar_t *pRead, bool *bVk);
bool getCursor(short *pX, short *pY);
bool setCursor(short x, short y);
bool getCharacter(short x, short y, wchar_t *pRead);

#endif // !TERMI*NAL
