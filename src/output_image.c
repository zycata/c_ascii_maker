#include "output_image.h"
#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define VALUE_CHARS " .-=+*x#$&X@"
#define N_VALUES (sizeof(VALUE_CHARS) - 1)



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

// for all intents for practical coding what im doing here is completely unnecessary and literally does not
// benefit from anything BUT this is KINDAAA cool
void output_the_image(image_information* img, double sobel_threshhold, FILE* outstream) {
    for (size_t y = 0; y < img->height; y++) {
        for (size_t x = 0; x < img->width; x++) {
            double sobel_x = get_sobel_x(img, x, y);
            double sobel_y = get_sobel_y(img, x, y);
            double sobel_magnitude = sqrt(sobel_x * sobel_x + sobel_y * sobel_y);
            double sobel_angle = atan2(sobel_y, sobel_x) * 180.0 / 3.14159;
            char character_to_use = sobel_magnitude > sobel_threshhold? get_sobel_angle_char(sobel_angle) : get_ascii_char(get_grayscale_from_pixel(img, x, y) / 255.0);

            if (img->channels < 3) {
                fprintf(outstream,"%c", character_to_use);
            } else {
                double* pixel = get_pixel(img, x, y);
                int r = pixel[0];
                int g = pixel[1];
                int b = pixel[2];
                fprintf(outstream ,"\x1b[38;2;%d;%d;%dm%c", (int) r, (int) g, (int) b, character_to_use);
            }
        }
        fprintf(outstream, "\n");
    }

    // fprintf(outstream, "\x1b[0m"); realized i dont need this if I'm just... not printing anything afterwards
}

void out_image(image_information* img, double brighten_amount, double sobel_threshhold, int color_option, const char* out_filename) {
    FILE* outstream = stdout;
    if (color_option == 0) {
        convert_to_grayscale(img);
    } 
    brighten_image(img, brighten_amount);
   
    if (out_filename != NULL) {
        outstream = fopen(out_filename, "w");
        if (!outstream) {
            fprintf(stderr, "failed to open or create file: %s\n", out_filename);
            return;
        }
    }
    output_the_image(img,  sobel_threshhold, outstream);

    if (out_filename != NULL) {
        fclose(outstream);
        printf("successfully written to: %s\n", out_filename);
        color_option == 1 ? printf("You may need to use a terminal viewer to see color -> use cat %s to view in your terminal\n", out_filename): printf("\n");
    }
    
}