// certified command line arguments momentos
#include <stdio.h>
#include <stdlib.h>
#include "../include/cla_parse.h"
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#define DEFAULT_MAX_WIDTH 96
#define DEFAULT_MAX_HEIGHT 64
#define DEFAULT_SOBEL_EDGE_THRESHOLD 67.0 // im so funny but this value actually kinda works
#define DEFAULT_COLOR_OPTION 1 // should have color
#define DEFAULT_CHARACTER_RATIO 2.0
#define DEFAULT_BRIGHTEN_AMOUNT 1.10

void test_system(void) {
#ifdef _WIN32
    printf("You are currently on a Windows Machine\n");


#else
    printf("You are currently not on a Windows Machine (Probably linux)");

#endif
    printf("\n");
    return;
}


// obtains terminal height
int get_terminal_size(size_t* width, size_t* height) {
#ifdef _WIN32
    if (!_isatty(0)) {
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


void print_help(const char* execname) {
    printf("Usage -h or -H for help\n");
    printf("Usage: %s path/to/desired_image.jpg -other -flags\n", execname);
    printf("Usage to read an outputted file: %s -r path/to/outputed\n", execname);
    printf("-----Flags-----\n");
    printf("- -mw maximum width (Default terminal height or %d characters) \n", DEFAULT_MAX_WIDTH);
    printf("- -mh maximum height (Default terminal height or %d characters) \n", DEFAULT_MAX_HEIGHT);
    printf("- -set Sobel edge detect threshold (DEFAULT: %lf) \n", DEFAULT_SOBEL_EDGE_THRESHOLD);
    printf("- -cr Character Ratio (Default %lf)\n", DEFAULT_CHARACTER_RATIO);
    printf("- -ba Brighten image amount (Default %lf) \n", DEFAULT_BRIGHTEN_AMOUNT);
    printf("- -o Output to ASCII to file (DEFAULT OFF)\n");
    printf("- --usebw use black and white (Default OFF) \n");
}   
// must give a pre allocated arguments returns 1 if successful and 0 if not
int parse_arguments(int argc, char* argv[], args_list* arguments) {

    if (argc==1) {
        fprintf(stderr, "Not enough command line arguments, use -h for help\n");
        
        return 0;
    }
    if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "-H")) {
        print_help(argv[0]);
        return 0;
    } else if (!strcmp(argv[1], "-r") || !strcmp(argv[1], "-R")) {

        if (argc != 3) {
            fprintf(stderr, "In complete arguments usage for reading:\n%s -r [filename]\n", argv[0]);
            return 0;
        } 

        arguments->file_path = argv[2];
        return 2;
    }

    // first argument should be the filepath
    arguments->file_path = argv[1];


    arguments->max_width = DEFAULT_MAX_WIDTH;
    arguments->max_height = DEFAULT_MAX_HEIGHT;
    arguments->edge_sobel_threshold = DEFAULT_SOBEL_EDGE_THRESHOLD;
    arguments->color_option = DEFAULT_COLOR_OPTION;
    arguments->character_ratio = DEFAULT_CHARACTER_RATIO;
    arguments->brighten_amount = DEFAULT_BRIGHTEN_AMOUNT;

    arguments->output_file_path = NULL;
    
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
        else if (!strcmp(argv[i], "-set") && i + 1 < argc) {
            arguments->edge_sobel_threshold = atof(argv[++i]);
        }
        else if (!strcmp(argv[i], "-ba") && i + 1 < argc) {
            arguments->brighten_amount = atof(argv[++i]);
        } 
        else if (!strcmp(argv[i], "-cr") && i + 1 < argc) {
            arguments->character_ratio = atof(argv[++i]);
        } 
        else if (!strcmp(argv[i], "--usebw")) {
            arguments->color_option = 0;
        }
        else if (!strcmp(argv[i], "-o") && i + 1 < argc) {
            // pass but make it output to a .txt file
            arguments->output_file_path = argv[++i];
        }
        else {
            fprintf(stderr, "Warning: Ignoring invalid or incomplete argument '%s'\n", argv[i]);
            return 0;
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
    printf("  Output File Path:       %s\n", args->output_file_path != NULL? args->file_path : "(NULL)");
    printf("--------------------------\n");
}
