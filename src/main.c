#include <stdio.h>
#include <stdlib.h>

#include "rc_malloc.h"
#include "cla_parse.h"
#include "image.h"
#include "output_image.h"


void produce_output(args_list* arguments);

int main(int argc, char** argv) {

    

    args_list* args = rc_malloc(sizeof(args_list));
    int result = parse_arguments(argc, argv, args);
    if (result == 0) {
        printf("Incomplete arguments\n");
        return 1;
    } else if (result == 2) {
        print_ascii_file(args->file_path);
        return 0;
    }

    
    
    produce_output(args);

    rc_free_ref(args);
    return 0;
}


void produce_output(args_list* args) {
    image_information* img = load_image(args->file_path);
    if (!img) {
        return;
    }
    image_information* resized = resize_image(img, args->max_width, args->max_height, args->character_ratio);

    out_image(resized, args->brighten_amount, args->edge_sobel_threshold, args->color_option, args->output_file_path);
    
    rc_free_image_info(resized);
    rc_free_image_info(img);
    
}
