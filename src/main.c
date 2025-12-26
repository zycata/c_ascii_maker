#include <stdio.h>
#include <stdlib.h>

#include "rc_malloc.h"
#include "cla_parse.h"
#include "image.h"


void test_image() {
    image_information* img = load_image("white.jpg");
    print_image_information(img);

    // omg my rc coutning does work omg yes yes valgrind sdaid so
    rc_free_ref(img->data);
    rc_free_ref(img);
}

int main(int argc, char** argv) {

    printf("argc: %d\n", argc);
    for (int i=0; i < argc; i++) {
        printf("%s ", argv[i]);
    }


    int* reals = rc_malloc(sizeof(int)*10);

    printf("\nreference count: %lu \n", rc_get_refs(reals));


    rc_free_ref(reals);

    test_system();

    size_t* width = malloc(sizeof(*width));
    size_t* height = malloc(sizeof(*height));

    if (!get_terminal_size(width, height)) {
        printf("failed to get terminal size");
        return 0;
    }
    printf("terminal width: %zu, height: %zu \n", *width, *height);

    free(height);
    free(width);


    // ainsi escape sequencing
    int r = 0xD3;
    int g = 0;
    int b = 0xFF;
    printf("\x1b[38;2;%d;%d;%dm%s\n", r, g, b, "what the fuck is a kilometer");
    printf("\x1b[0m");


    args_list* s = rc_malloc(sizeof(args_list));
    
    parse_arguments(argc, argv, s);
    print_arguments(s);
    rc_free_ref(s);

    return 0;
}