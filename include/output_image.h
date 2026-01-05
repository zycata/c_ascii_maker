#ifndef OUTPUT_IMAGE
#define OUTPUT_IMAGE

#include "image.h"
#include "cla_parse.h"



void out_image(image_information* img, double brighten_amount, double sobel_threshhold, int color_option, const char* out_filename);

void print_ascii_file(const char* filepath);

void produce_output(args_list* args);

#endif

