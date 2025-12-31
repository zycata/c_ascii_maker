#include "print_image.h"
#include "image.h"

#include <stdio.h>
#include <stdlib.h>
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

char get_sobel_angle_char(double sobel_angle) {
    if ((22.5 <= sobel_angle && sobel_angle <= 67.5) || (-157.5 <= sobel_angle && sobel_angle <= -112.5))
        return '/';
    else if ((67.5 <= sobel_angle && sobel_angle <= 112.5) || (-112.5 <= sobel_angle && sobel_angle <= -67.5))
        return '_';
    else if ((112.5 <= sobel_angle && sobel_angle <= 157.5) || (-67.5 <= sobel_angle && sobel_angle <= -22.5))
        return '\\';
    else
        return '|';
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
            
            

            char character_to_use;

            double sobel_x, sobel_y;
            if (_x == 0 || _x == img->width - 1 || _y==0 || _y == img->height - 1) {
                sobel_x = 0;
                sobel_y = 0;
            } else {
                sobel_x = get_sobel_x(img, _x, _y);
                sobel_y = get_sobel_y(img, _x, _y);
                
            }

            double sobel_magnitude = sqrt(sobel_x*sobel_x + sobel_y*sobel_y);
            double sobel_angle = atan2(sobel_y, sobel_x) * 180.0 / 3.14159;
            // printf("Angle %lf Magnitude%lf\n", sobel_angle, sobel_magnitude);

            if (sobel_magnitude > sobel_threshhold) {
                character_to_use = get_sobel_angle_char(sobel_angle);
            } else {
                character_to_use = get_ascii_char(get_grayscale_from_rgb(r,g,b));
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
