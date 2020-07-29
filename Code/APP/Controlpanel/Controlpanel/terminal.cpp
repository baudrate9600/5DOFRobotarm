#include "terminal.h"
#include <stdio.h>
#include <wchar.h>
#include <windows.h>

int size_x, size_y;
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
int enable_terminal() {
 // Set output mode to handle virtual terminal sequences
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return GetLastError();
    }
   // SMALL_RECT windowSize = { 0 , 0 , 77 , 47 }; //change the values
   // SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
}
void set_window_size(int left, int top, int right, int bottom) {
     SMALL_RECT windowSize = { left , top , right , bottom }; //change the values
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
}
void save_position() {
     wprintf(L"%s[s", ESC);
}
void last_position() {
    wprintf(L"%s[u", ESC);
}
void move_xy(int x, int y) {
   
    wprintf(L"%s[%d;%df",ESC,y,x);
}
void set_labels();
