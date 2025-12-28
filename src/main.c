#include <stdio.h>
#include <stdlib.h>

#include "rc_malloc.h"
#include "cla_parse.h"
#include "image.h"
#include "print_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stbi/stb_image_write.h"
void test_image() {
    image_information* img = load_image("lightning.jpg");
    print_image_information(img);

    image_information* resized =  resize_image(img, 1500, 3000, 2.0);

    print_image_information(resized);

    unsigned char* data = rc_malloc(sizeof(*data)* resized->data_size);
    for (size_t i = 0; i < resized->data_size; i++) {
        data[i] = (unsigned char) resized->data[i];
    }
    stbi_write_jpg("shrunk.jpg", resized->width, resized->height, resized->channels, data, 100);
    // omg my rc coutning does work omg yes yes valgrind sdaid so
    rc_free_image_info(img);    
    rc_free_image_info(resized);

    rc_free_ref(data);

}

void output_image(args_list* arguments);

int main(int argc, char** argv) {

    printf("argc: %d\n", argc);
    for (int i=0; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    test_system();



    args_list* s = rc_malloc(sizeof(args_list));
    
    parse_arguments(argc, argv, s);
    print_arguments(s);
    

    output_image(s);

    rc_free_ref(s);
    return 0;
}


void output_image(args_list* arguments) {
    image_information* img = load_image(arguments->file_path);
    if (!img) {
        return;
    }
    image_information* resized = resize_image(img, arguments->max_width, arguments->max_height, arguments->character_ratio);

    print_brightened_image(resized, arguments->brighten_amount);

    rc_free_image_info(resized);
    rc_free_image_info(img);


}