// certified command line arguments momentos
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>

#endif



void test_system() {
#ifdef _WIN32
    printf("OMG WINDOWS MOMENT OMG");


#else
    printf("OMG LINUX LINUX REAL REAL");

#endif
    printf("\n");
    return;
}

int get_terminal_size(size_t* width, size_t* height) {
#ifdef _WIN32
    if (!isatty(0)) {
        return 0;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return 0;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return 0;

    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return 1;
#else
    if (isatty(0)) {
        return 0;
    }
    struct winsize ws;

    if (ioctl(0, TIOCGWINSZ, &ws) == 0) {
        *width = (size_t) ws.ws_col;
        *height = (size_t) ws.ws_row;
        return 1;
    }

#endif
    return 0;
}