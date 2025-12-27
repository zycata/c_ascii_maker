#include <stdio.h>
#include <stdlib.h>

#include "rc_malloc.h"
#include "cla_parse.h"
#include "image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stbi/stb_image_write.h"
void test_image() {
    image_information* img = load_image("white.jpg");
    print_image_information(img);

    image_information* resized =  resize_image(img, 1500, 3000, 2.0);

    print_image_information(resized);

    unsigned char* data = rc_malloc(sizeof(*data)* resized->data_size);
    for (size_t i = 0; i < resized->data_size; i++) {
        data[i] = (unsigned char) resized->data[i];
    }
    stbi_write_jpg("shrunk.jpg", resized->width, resized->height, resized->channels, data, 100);
    // omg my rc coutning does work omg yes yes valgrind sdaid so
    rc_free_ref(img->data);
    rc_free_ref(img);
    
    rc_free_ref(resized->data);
    rc_free_ref(resized);
    rc_free_ref(data);

}

int main(int argc, char** argv) {

    printf("argc: %d\n", argc);
    for (int i=0; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    test_system();

    size_t* width = malloc(sizeof(*width));
    size_t* height = malloc(sizeof(*height));

    if (!get_terminal_size(width, height)) {
        printf("failed to get terminal size");
        return 0;
    }
    printf("terminal width: %zu, height: %zu \n", *width, *height);

    calculate_new_dimensions(width, height, (size_t) 3000,(size_t) 4499,(size_t) 1500,(size_t) 3000, 2.0);
    printf("new width: %zu, height: %zu \n", *width, *height);
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

     test_image();
    return 0;
}