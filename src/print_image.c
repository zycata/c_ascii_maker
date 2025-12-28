#include "print_image.h"
#include "image.h"

#include <stdio.h>

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

// omg diy min function??? 
double find_min(double a, double b) {
    return a < b ? a : b;
}
void brighten(double *r, double *g, double* b, double amount) {
    *r = find_min(255.0, (*r) * amount);
    *g = find_min(255.0, (*g) * amount);
    *b = find_min(255.0, (*b) * amount);
}
void print_brightened_image(image_information* img, double brighten_amount) {
    


    for (size_t _y = 0; _y < img->height; _y++) {
        for (size_t _x = 0; _x < img->width; _x++) {
            double* pixel = get_pixel(img, _x, _y);

            double r = pixel[0];
            double g = pixel[1];
            double b = pixel[2];
            brighten(&r, &g, &b, brighten_amount);
            char character_to_use = get_ascii_char(get_grayscale_from_rgb(r, g, b));
            
            printf("\x1b[38;2;%d;%d;%dm%c", (int) r, (int) g, (int) b, character_to_use);
            
        }
        printf("\n");
    }

    printf("\x1b[0m");
}
