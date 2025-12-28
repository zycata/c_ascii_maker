// certified command line arguments momentos
#include <stdio.h>
#include <stdlib.h>
#include "cla_parse.h"
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#define DEFAULT_MAX_WIDTH = 96
#define DEFAULT_MAX_HEIGHT = 64
#define DEFAULT_SOBEL_EDGE_THRESHOLD = 4.0
#define DEFAULT_COLOR_OPTION = 1 // should have color
#define DEFAULT_CHARACTER_RATIO = 2.0
#define DEFAULT_BRIGHTEN_AMOUNT = 1.25

void test_system() {
#ifdef _WIN32
    printf("OMG WINDOWS MOMENT OMG");


#else
    printf("OMG LINUX LINUX REAL REAL");

#endif
    printf("\n");
    return;
}


// obtains terminal height
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
    if (!isatty(0)) {
        
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



// must give a pre allocated arguments
int parse_arguments(int argc, char* argv[], args_list* arguments) {

    if (argc==1) {
        fprintf(stderr, "Not enough command line arguments\n");
        
        return 0;
    }


    // first argument should be the filepath
    arguments->file_path = argv[1];


    arguments->max_width  DEFAULT_MAX_WIDTH;
    arguments->max_height  DEFAULT_MAX_HEIGHT;
    arguments->edge_sobel_threshold DEFAULT_SOBEL_EDGE_THRESHOLD;
    arguments->color_option DEFAULT_COLOR_OPTION;
    arguments->character_ratio DEFAULT_CHARACTER_RATIO;
    arguments->brighten_amount DEFAULT_BRIGHTEN_AMOUNT;
    
    size_t width, height;

    if(!get_terminal_size(&width, &height)) {
        fprintf(stderr, "Failed to get terminal dimensions, using default sizes...\n");
    }

    arguments->max_width  = width;
    arguments->max_height = height;

    for (int i = 2; i < argc; ++i) {
        if(!strcmp(argv[i], "-mw") && i + 1 < argc) {
            arguments->max_width = strtoul(argv[++i], NULL, 0);
        } 
        else if (!strcmp(argv[i], "-mh") && i + 1 < argc) {
            arguments->max_height = strtoul(argv[++i], NULL, 0);
        } 
        else if (!strcmp(argv[i], "-esl") && i + 1 < argc) {
            arguments->edge_sobel_threshold = atof(argv[++i]);
        }
        else if (!strcmp(argv[i], "-ba") && i + 1 < argc) {
            arguments->brighten_amount = atof(argv[++i]);
        } 
        else if (!strcmp(argv[i], "--usebw")) {
            arguments->color_option = 0;
        } 
        else {
            fprintf(stderr, "Warning: Ignoring invalid or incomplete argument '%s'\n", argv[i]);
        }
    }

    

    // successfully parsed arguments
    return 1;



}


void print_arguments(const args_list* args) {
    // Check for a NULL pointer to prevent a crash
    if (args == NULL) {
        printf("Error: print_args_list received a NULL pointer.\n");
        return;
    }

    printf("--- args_list Contents ---\n");
    printf("  File Path:              %s\n", args->file_path != NULL ? args->file_path : "(NULL)");
    printf("  Max Width (Chars):      %zu\n", args->max_width);
    printf("  Max Height (Chars):     %zu\n", args->max_height);
    printf("  Edge Sobel Threshold:   %lf\n", args->edge_sobel_threshold);
    printf("  Color Option:           %d\n", args->color_option);
    printf("  Character Ratio:        %lf\n", args->character_ratio);
    printf("  Brighten Amount:        %lf\n", args->brighten_amount);
    printf("--------------------------\n");
}