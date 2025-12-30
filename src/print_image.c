#include "print_image.h"
#include "image.h"

#include <stdio.h>
#include <math.h>
#define VALUE_CHARS " .-=+*x#$&X@"
#define N_VALUES (sizeof(VALUE_CHARS) - 1)






double get_grayscale_from_rgb(double r, double g, double b) {
    double divisor = 255.0;

    double gamma =  0.2126 * r + 0.7152 * g + 0.0722 * b ;

    return gamma/divisor;
}

char get_ascii_char(double grayscale) {
    size_t index = (size_t) (grayscale * (double) N_VALUES);

    
    if (index >= N_VALUES) {
        index = N_VALUES - 1;
    }

    return VALUE_CHARS[index];
}

// todo finish later
int get_sobel_char(char* character, double sobel_x, double sobel_y, double threshold) {
    
    if ((sobel_x < -1*threshold && sobel_y < -1*threshold)) {
        *character = '/';
    } 
    
    else if (sobel_x > threshold && sobel_y > threshold) {
        *character = '\\';
    }
    else if (fabs(sobel_x) > threshold) {
        (*character) = '|';
    } else if (fabs(sobel_y) > threshold) {
        (*character) = '_';
    } else {
        return 0;
    }


    return 1;
}

// omg diy min function??? 
double find_min(double a, double b) {
    return a < b ? a : b;
}
void brighten(double *r, double *g, double* b, double amount) {
    *r = find_min(255.0, (*r) * amount);
    *g = find_min(255.0, (*g) * amount);
    *b = find_min(255.0, (*b) * amount);
}
void print_brightened_image(image_information* img, double brighten_amount, double sobel_threshhold, int color_option) {
    


    for (size_t _y = 0; _y < img->height; _y++) {
        for (size_t _x = 0; _x < img->width; _x++) {
            double* pixel = get_pixel(img, _x, _y);

            double r = pixel[0];
            double g = pixel[1];
            double b = pixel[2];
            brighten(&r, &g, &b, brighten_amount);
            
            double sobel_x = get_sobel_x(img, _x, _y);
            double sobel_y = get_sobel_y(img, _x, _y);

            char character_to_use;

            
            if (!get_sobel_char(&character_to_use, sobel_x, sobel_y, sobel_threshhold)) {
                character_to_use = get_ascii_char(get_grayscale_from_rgb(r, g, b));
            } 
            
            
            if (!color_option) {
                printf("%c", character_to_use);
                continue;
            }
            printf("\x1b[38;2;%d;%d;%dm%c", (int) r, (int) g, (int) b, character_to_use);
            
        }
        printf("\n");
    }

    printf("\x1b[0m");
}
