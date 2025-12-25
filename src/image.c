
#define STB_IMAGE_IMPLEMENTATION
#include "image.h"
#include "rc_malloc.h"
#include "stb_image.h"
#include <stdio.h>


void print_image_information(image_information* img) {
    rc_keep_ref(img);
    printf("Printing image data:\nwidth: %zu height: %zu channels %zu total data size: %zu\n", 
        img->width, img->height, img->channels, img->data_size);
    if (img->data_size > 3) {
        printf("first 3 values%lf %lf %lf\n", img->data[0], img->data[1],img->data[2]);
    }
    rc_free_ref(img);
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