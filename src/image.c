
#include "image.h"
#include "rc_malloc.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stbi/stb_image.h"
#include <stdio.h>



void print_image_information(image_information* img) {
    printf("\n"); 
    printf("---   Image Information Summary ---\n");

    printf("  Width:            %6zu pixels\n", img->width);
    printf("  Height:           %6zu pixels\n", img->height);
    printf("  Channels:         %6zu\n", img->channels);
    printf("  Total Data Size:  %6zu elements\n", img->data_size);

    size_t data_to_show = 6;
    if (img->data_size > 0 && img->data != NULL) {
        printf("\n");
        printf("  Pixel Data Sample (First %zu elements):\n", img->data_size < data_to_show ? img->data_size : data_to_show);
        
        for (size_t i = 0; i < img->data_size && i < data_to_show; ++i) {
            printf("    Data[%zu]: %10.4lf\n", i, img->data[i]);
        }
        
        if (img->data_size > data_to_show) {
            printf("    ...\n"); 
        }
    } else {
        printf("\n");
        printf("  Pixel Data:       (Data array is NULL or size is 0)\n");
    }
    
    printf("--------------------------------------\n");
}

image_information* load_image(const char* file_path) {
    int width, height, channels;
    unsigned char* raw_image_data = stbi_load(file_path, &width, &height, &channels, 0);
    
    if (!raw_image_data) {
        fprintf(stderr, "Error: Failed to load image '%s': %s!\n", file_path, stbi_failure_reason());
        return NULL;
    }
    image_information* img_info = rc_malloc(sizeof(image_information));

    // the amount of entries in raw_image_data -> raw_image_data.size()
    size_t total_size = width * height * channels;  
    double* data = rc_malloc(sizeof(double) * total_size);

    // if any allocation failed
    if (!data || !img_info) {
        fprintf(stderr, "Failed to allocate memory for image data\n");
        return NULL;
    }
    for (size_t i = 0; i < total_size; i++) {
        data[i] = (double) raw_image_data[i];
    }
    stbi_image_free(raw_image_data);
    
    img_info->width = (size_t) width;
    img_info->height = (size_t) height;
    img_info->channels = (size_t) channels;
    
    img_info->data = data;
    // literally useless and counterintuitive but i wanted to use reference counting
    rc_keep_ref(img_info->data);
    rc_free_ref(data);

    img_info->data_size = total_size;
    return img_info;
}


double* get_pixel(image_information* image, size_t x, size_t y) {
    return &image->data[(y * image->width + x) * image->channels];
}

// new_pixel size should be the same size as the channel size
void set_pixel(image_information* image, size_t x, size_t y, double* new_pixel) {
    double* old_pixel = get_pixel(image, x, y);
    for (size_t i = 0; i < image->channels; i++) {
        old_pixel[i] = new_pixel[i];
    }
}
// x1 < x2 and y1 < y2, writes to average_pixel
// ok so turns out this shit actually sucks at resizing images from like something like 4000x3000 -> 3000x2000 if the ratios are too close the thing fucks up
// so like atleast a 2x ratio is needed
void write_average_pixel(image_information* img, double* average_pixel, size_t x1, size_t x2, size_t y1, size_t y2) {
    double total_pixels = (double) (x2-x1) * (y2 - y1);

    if (total_pixels == 0) {
        // fuckass cant code for shi
        total_pixels = 1;
    }
    // fuck i made rc_malloc so i will refuse to use calloc
    // also i realiezd that because this is single threaded rc_malloc is just lowkey kinda useless #thought ahead
    for (size_t i = 0; i < img->channels; i++) {
        average_pixel[i] = 0;
    }
    for (size_t _x = x1; _x < x2; _x++) {
        for (size_t _y = y1; _y < y2; _y++) {
            double* pixel = get_pixel(img, _x, _y);

            for (size_t i = 0; i < img->channels; i++) {
                average_pixel[i] += pixel[i];
            }
        }
    }

    

    for (size_t i = 0; i < img->channels; i++) {
        average_pixel[i] = average_pixel[i] / total_pixels;
    }

}


// this function does not account if the max width after the resize because i cant do baisc fucking math
void calculate_new_dimensions(size_t* new_width, size_t* new_height, size_t original_width, size_t original_height, size_t max_width, size_t max_height, double terminal_ratio) {
    size_t proposed_width, proposed_height;

    proposed_width = original_width;

    proposed_height = original_height / terminal_ratio;
    // printf("%lu, %lu\n", proposed_height, max_height);
    if (proposed_width > max_width || proposed_height > max_height) {
        // do nothing 
        proposed_height = max_height;
        double new = (terminal_ratio * original_width) / (double) ((double) original_height / (double) max_height);

        proposed_width = new;
    } else {
        printf("Not resizing the image\n");
    }
    *new_width = proposed_width;
    *new_height = proposed_height;

}
// yeah i kinda suck at allocating memory actually
// yeah uhh it doesnt work if the new image size is too similar to the old one :skull: i hope that scenario NEVER happens 
image_information* resize_image(image_information* original, size_t max_width, size_t max_height, double terminal_ratio) {
    size_t new_width, new_height;
    size_t channels = original->channels;

    calculate_new_dimensions(&new_width, &new_height, original->width, original->height, max_width, max_height, terminal_ratio);

    // so TECHNICALLY this fixes the problem when the resized ratio is too similar
    // however this solution sucks shit but im also gonna keep it
    // !!! todo: offload this to calculate_new_dimensions
    double ratio_x = original->width / new_width;
    double ratio_y = original->height / new_height;
    if (ratio_x < 2.0 || ratio_y < 2.0) {
        new_width = original->width / 2;
        new_height = original->height / (2*terminal_ratio);
    }

    size_t resized_total_data_size = new_width * new_height * channels;
    double* resized_image_data = rc_malloc(resized_total_data_size * sizeof(double));
    
    /*
        uhh so this is actually kinda neat
        let's say you have new_height = 20 and original_height is 50
        when y = 1, then 1 * 50 / 20 = 2 and (1+1) * 50 / 20 is 5
        then at the the two edge cases where:
        y = 19 (which is new_height - 1) 19 * 50 / 20 = 47 and (19+1) * 50 / 20 = 50
        y = 0 where 0 * 50 / 20 = 0 and (0+1) * 50 / 20 = 2 where it kind of automatically wraps around the edges
        super cool and it takes advantage of unsigned non-floating point numbers rounding down in c
        yeah i'd never be able to think of this shit in an interview
    */
    for (size_t _y = 0; _y < new_height; _y++) {
        size_t y1 = (_y * original->height) / (new_height);
        size_t y2 = ((_y + 1) * original->height) / (new_height);

        for (size_t _x = 0; _x < new_width; _x++) {
            size_t x1 = (_x * original->width) / (new_width);
            size_t x2 = ((_x + 1) * original->width) / (new_width);

            write_average_pixel(original, &resized_image_data[(_x + _y * new_width) * channels], x1, x2, y1, y2);
        }
    }

    // get_average_pixel(original, &resized_image_data[0], 0, 2, 0, 2);



    image_information* resized_image = rc_malloc(sizeof(image_information));
    resized_image->width = new_width;
    resized_image->height = new_height;
    resized_image->channels = channels;
    resized_image->data_size = resized_total_data_size;
    resized_image->data = resized_image_data;

    return resized_image;
}

