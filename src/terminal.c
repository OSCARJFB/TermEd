#include "terminal.h"

#ifdef _DEBUG
    static HANDLE getStdHandle(int STD_HANDLE)
    {
        return GetStdHandle(STD_HANDLE);
    }
#else 
    #define getStdHandle(x) GetStdHandle(x);
#endif // DEBUG


DWORD g_dOriginalConsoleMode = 0;
static resetTerminal(DWORD dMode)
{
    HANDLE hInput = getStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hInput, dMode);
}

bool init(void)
{
    DWORD dMode;
    HWND hWnd = GetConsoleWindow();
    HANDLE hInput = getStdHandle(STD_INPUT_HANDLE);
    
    if (!hWnd || hInput == INVALID_HANDLE_VALUE)
        return false;

    if (!GetConsoleMode(hInput, &dMode))
        return false;

    g_dOriginalConsoleMode = dMode;
    atexit(resetTerminal);

    dMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    if (!SetConsoleMode(hInput, dMode))
        return false;
}

bool clear(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT scrollRect;
    COORD dwDestinationOrigin, dwCursorPosition;
    CHAR_INFO fill;
    HANDLE hOutput = getStdHandle(STD_OUTPUT_HANDLE);

    if (hOutput == INVALID_HANDLE_VALUE)
        return false;

    if (!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return false;

    scrollRect.Left = scrollRect.Top = 0;
    scrollRect.Right = csbi.dwSize.X;
    scrollRect.Bottom = csbi.dwSize.Y;
   
    dwDestinationOrigin.X = 0;
    dwDestinationOrigin.Y = 0 - csbi.dwSize.Y;

    fill.Char.UnicodeChar = TEXT(' ');
    fill.Attributes = csbi.wAttributes;

    dwCursorPosition.X = 0;
    dwCursorPosition.Y = 0;

    ScrollConsoleScreenBuffer(hOutput, &scrollRect, NULL, dwDestinationOrigin, &fill);
    SetConsoleCursorPosition(hOutput, dwCursorPosition);
    return true;
}

bool write(wchar_t cWrite)
{
    DWORD dWrittenCount = 0;
    HANDLE hOutput = getStdHandle(STD_OUTPUT_HANDLE);

    if (hOutput == INVALID_HANDLE_VALUE)
        return false;

    if (!WriteConsoleW(hOutput, &cWrite, 1, &dWrittenCount, NULL) || dWrittenCount != 1)
        return false;
    return true;
}

static WORD getVK(WORD wVirtualKeyCode)
{
    switch (wVirtualKeyCode)
    {
    case VK_LEFT:
        break;
    case VK_RIGHT:
        break;
    case VK_UP:
        break;
    case VK_DOWN:
        break;
    default:
        break;
    }
}

bool read(wchar_t *pRead, bool *pVk)
{
    DWORD dRead = 0;
    HANDLE hInput = getStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;

    *pRead = '\0';
    *pVk = false;

    if (hInput == INVALID_HANDLE_VALUE)
        return false;

    if (!ReadConsoleInput(hInput, &inputRecord, 1, &dRead) || dRead != 1)
        return false;
    
    if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
        if (inputRecord.Event.KeyEvent.uChar.UnicodeChar) {
            *pRead = inputRecord.Event.KeyEvent.uChar.UnicodeChar;
        }
        if (inputRecord.Event.KeyEvent.wVirtualKeyCode <= VK_DOWN && inputRecord.Event.KeyEvent.wVirtualKeyCode >= VK_LEFT) {
            *pRead = getVK(inputRecord.Event.KeyEvent.wVirtualKeyCode);
            *pVk = true;
        }
    }

    return *pRead != '\0';
}

bool getCursor(short *pX, short *pY)
{
    COORD coord;
    HANDLE hOutput = getStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (hOutput  == INVALID_HANDLE_VALUE)
        return false;

    if (!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return false;
        
    coord = csbi.dwCursorPosition;
    *pX = coord.X;
    *pY = coord.Y;
    return true;
}

bool setCursor(short x, short y)
{
    COORD coord = { x, y };
    HANDLE hOutput = getStdHandle(STD_OUTPUT_HANDLE);

    if (hOutput == INVALID_HANDLE_VALUE)
        return false;

    if (!SetConsoleCursorPosition(hOutput, coord))
        return false;
    return true;
}

bool getCharacter(short x, short y, wchar_t *pRead)
{
    COORD coord = { x, y };
    DWORD dRead = 0;
    wchar_t c;
    HANDLE hOutput = getStdHandle(STD_OUTPUT_HANDLE);

    if (hOutput == INVALID_HANDLE_VALUE)
        return false;

    if (!ReadConsoleOutputCharacterW(hOutput, &c, 1, coord, &dRead))
        false;

    *pRead = c;
    return true;
}