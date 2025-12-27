

#ifndef C_IMAGE
#define C_IMAGE
#include <stdlib.h>

typedef struct {
    size_t width;
    size_t height;
    size_t channels;
    double* data;
    size_t data_size;

} image_information;

void print_image_information(image_information* img); 
image_information* load_image(const char* file_path);

void calculate_new_dimensions(size_t* new_width, size_t* new_height, size_t original_width, size_t original_height, size_t max_width, size_t max_height, double terminal_ratio);
image_information* resize_image(image_information* original, size_t max_width, size_t max_height, double terminal_ratio);
#endif