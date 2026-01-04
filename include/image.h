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

void rc_free_image_info(image_information* img);

void print_image_information(image_information* img); 
image_information* load_image(const char* file_path);

double* get_pixel(image_information* image, size_t x, size_t y);

void calculate_new_dimensions(size_t* new_width, size_t* new_height, size_t original_width, size_t original_height, size_t max_width, size_t max_height, double terminal_ratio);
void resize_image(image_information* original, size_t max_width, size_t max_height, double terminal_ratio);

double get_grayscale_from_pixel(image_information* img, size_t x, size_t y);
int convert_to_grayscale(image_information* img);
void brighten_image(image_information* img, double brighten_amount);

double get_convolution_value_from_kernel(image_information* img, size_t x, size_t y, 
                                    double* kernel, int kernel_rows, int kernel_cols,
                                    double (*kernel_func) (image_information*, size_t, size_t ));

double get_sobel_x(image_information* img, size_t x, size_t y);
double get_sobel_y(image_information* img, size_t x, size_t y);
#endif
