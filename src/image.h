

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

#endif