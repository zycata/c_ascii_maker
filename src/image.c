#include "../include/image.h"
#include "../include/rc_malloc.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <stdio.h>

#define SOBEL_X {-1, 0, 1, -2, 0, 2, -1, 0, 1}
#define SOBEL_X_ROWS 3
#define SOBEL_X_COLS 3

#define SOBEL_Y {-1, -2, -1, 0, 0, 0, 1, 2, 1}
#define SOBEL_Y_ROWS 3
#define SOBEL_Y_COLS 3

void rc_free_image_info(image_information* img) {
    rc_free_ref(img->data);
    rc_free_ref(img);
}

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
    if (x > image->width-1 || y > image->height-1) {
        return NULL;
    }
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
        // fuckass cant code for shit -> bug mentioned in comment below
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

        proposed_width = (size_t) new;

        // python who have some function called do_what_you_want that does what you want by reading ur mind
        if (proposed_width > max_width) {
            proposed_width = max_width;
            new = original_height / ((double) original_width / (double) max_width) / terminal_ratio;
            proposed_height = (size_t) new;
        }
    } else {
        printf("Not resizing the image as it fits the terminal constraints\n");
    }

    // so TECHNICALLY this fixes the problem when the resized ratio is too similar
    // however this solution sucks shit but im also gonna keep it
    double ratio_x = original_width / proposed_width;
    double ratio_y = original_height / proposed_height;
    if (ratio_x < 2.0 || ratio_y < 2.0) {
        proposed_width = original_width / 2;
        proposed_height = original_height / (2*terminal_ratio);
    }

    *new_width = proposed_width;
    *new_height = proposed_height;
    
}
// yeah i kinda suck at allocating memory actually
// yeah uhh it doesnt work if the new image size is too similar to the old one :skull: i hope that scenario NEVER happens 
// update: now resizes the images instead of making a new one
void resize_image(image_information* img, size_t max_width, size_t max_height, double terminal_ratio) {
    size_t new_width, new_height;
    size_t channels = img->channels;

    // lord forgive me for my sins
    calculate_new_dimensions(&new_width, &new_height, img->width, img->height, max_width, max_height, terminal_ratio);


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
        size_t y1 = (_y * img->height) / (new_height);
        size_t y2 = ((_y + 1) * img->height) / (new_height);

        for (size_t _x = 0; _x < new_width; _x++) {
            size_t x1 = (_x * img->width) / (new_width);
            size_t x2 = ((_x + 1) * img->width) / (new_width);

            // todo ?? multithread this (i need a reason for rc_malloc to exist)
            write_average_pixel(img, &resized_image_data[(_x + _y * new_width) * channels], x1, x2, y1, y2);
        }
    }

    // get_average_pixel(original, &resized_image_data[0], 0, 2, 0, 2);



    img->width = new_width;
    img->height = new_height;
    
    img->data_size = resized_total_data_size;

    rc_free_ref(img->data);
    img->data = resized_image_data;

    
}

// returns a value from 0 to 255.0
double get_grayscale_from_pixel(image_information* img, size_t x, size_t y) {
    if (img->channels < 3) {
        return get_pixel(img, x, y)[0];
    }
    double* pixel = get_pixel(img, x, y);
    double r = pixel[0];
    double g = pixel[1];
    double b = pixel[2];
    double gamma = 0.2126 * r + 0.7152 * g + 0.0722 * b ;
    return gamma;
}


// turns an image to a grayscale img
int convert_to_grayscale(image_information* img) {
    if (img->channels <= 2) {
        // no need to do it for already grayscale img
        return 1;
    }
    double* grayscale_data = rc_malloc(sizeof(double) * img->width * img->height);
    if (!grayscale_data) {
        return 0;
    }
    size_t pos = 0;;
    for (size_t y = 0; y < img->height; y++) {
        // i SPENT like 10 minutes debugging just to realized i wrote x > img->width instead of < bruhify
        for (size_t x = 0; x < img->width; x++) {
            grayscale_data[pos] = get_grayscale_from_pixel(img, x, y);
            pos++;
            
        } 
    }
    rc_free_ref(img->data);
    img->data = grayscale_data;
    img->channels = 1;
    img->data_size = img->height * img->width;
    return 1;
}


// omg diy min function??? 
double find_min(double a, double b) {
    return a < b ? a : b;
}
void brighten_image(image_information* img, double brighten_amount) {
    
    
    for (size_t y = 0; y < img->height; y++) {
        // i SPENT like 10 minutes debugging just to realized i wrote x > img->width instead of < bruhify
        for (size_t x = 0; x < img->width; x++) {
            double* pixel = get_pixel(img, x, y);
            for (size_t i = 0; i < img->channels; i++) {
                pixel[i] = find_min(pixel[i]*brighten_amount, 255.0);
            }
            
        } 
    }
   

}

// returns a value from -255.0 to 255.0 
// kernel must be a square matrice + flattened
// also literally no reason for me to pass in a function pointer i just REALLY wanted to use it for funnbvvnbn  
double get_convolution_value_from_kernel(image_information* img, size_t x, size_t y, 
                                    double* kernel, int kernel_rows, int kernel_cols,
                                    double (*kernel_func) (image_information*, size_t, size_t )) {
    size_t cur_item = 0;
    // size_t kernel_size = kernel_rows * kernel_cols;
    // double* total_items = rc_malloc(sizeof(*total_items)*kernel_size);
    
    // shift the start xy value to be offset to be the top of where the
    // kernel should be
    int start_x = x - (kernel_rows/2);
    int start_y = y - (kernel_cols/2);
    double convolution_sum = 0;
    // j for y, i for x
    // honestly i think i really could have done a better job with variable naming here
    for (int j = start_y; j <  start_y+kernel_rows; j++) {
        for (int i = start_x; i < start_x+kernel_cols; i++) {
            // ignore negative values becaause it can be negative due to my logic subtracting half of the kernel row
            if ((j < 0 || i < 0) || (get_pixel(img, i, j) == NULL)) {
                // pixel doesn't exist also 
                // honestly i do wish i could go back to python where it's just number instead of these data types
                
            } else {
                convolution_sum += kernel_func(img, i, j) * kernel[cur_item];
                cur_item++;
            }
            // fucklass logic
            
            
        }
    }
    
    return convolution_sum / (kernel_cols * kernel_rows);
}


double get_sobel_x(image_information* img, size_t x, size_t y) {
    if (x == 0 || x == img->width - 1 || y == 0 || y == img->height - 1) {
        return 0;
    }
    // sobel x kernel
    double sobel_kernal_x[] = SOBEL_X;
    return get_convolution_value_from_kernel(img, x, y, sobel_kernal_x, SOBEL_X_ROWS, SOBEL_X_COLS, get_grayscale_from_pixel);
}

double get_sobel_y(image_information* img, size_t x, size_t y) {
    // sobel y kernel
    if (x == 0 || x == img->width - 1 || y == 0 || y == img->height - 1) {
        return 0;
    }
    double sobel_kernal_y[] = SOBEL_Y;
    return get_convolution_value_from_kernel(img, x, y, sobel_kernal_y, SOBEL_Y_ROWS, SOBEL_Y_COLS, get_grayscale_from_pixel);
}
