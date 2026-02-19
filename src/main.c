#include <stdio.h>
#include "../include/cla_parse.h"
#include "../include/output_image.h"
#include "../include/rc_malloc.h"


int main(int argc, char** argv) {
    args_list args;
    int result = parse_arguments(argc, argv, &args);
    switch (result) {
        case 0:
            // invalid command line arguments
            break;
        case 1:
            // output an image normally
            produce_output(&args);
            break;
        case 2:
            // read from an outputted ascii image
            print_ascii_file(args.file_path);
            break;
        default:
            // funny stuff happens
            printf("unknown error occured when parsing command line arguments\n");
            return 1;
    }
    rc_free_ref(args.value_chars);
    return 0;
}
