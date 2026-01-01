#include <stdio.h>
#include <stdlib.h>

#include "rc_malloc.h"
#include "cla_parse.h"
#include "image.h"
#include "output_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stbi/stb_image_write.h"
void test_image(void) {
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

void poof(args_list* arguments);

int main(int argc, char** argv) {

    test_system();

    args_list* s = rc_malloc(sizeof(args_list));
    
    if(!parse_arguments(argc, argv, s)) {
        return 1;
    }
    
    print_arguments(s);
    

    poof(s);

    rc_free_ref(s);
    return 0;
}


void poof(args_list* args) {
    image_information* img = load_image(args->file_path);
    if (!img) {
        return;
    }
    image_information* resized = resize_image(img, args->max_width, args->max_height, args->character_ratio);

    out_image(resized, args->brighten_amount, args->edge_sobel_threshold, args->color_option, args->output_file_path);
    
    rc_free_image_info(resized);
    rc_free_image_info(img);
    
}
