#include "output_image.h"
#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define VALUE_CHARS " .-=+*x#$&X@"
#define N_VALUES (sizeof(VALUE_CHARS) - 1)




int String_EndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

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
            if (sobel_magnitude > sobel_threshhold) {
                character_to_use = get_sobel_angle_char(sobel_angle);
            } else {
                character_to_use = get_ascii_char(get_grayscale_from_rgb(r,g,b));
            }
            if (color_option == 0) {
                printf("%c", character_to_use);
                continue;
            }
            printf("\x1b[38;2;%d;%d;%dm%c", (int) r, (int) g, (int) b, character_to_use);
            
        }
        printf("\n");
    }

    printf("\x1b[0m");
}

// for all intents for practical coding what im doing here is completely unnecessary and literally does not
// benefit from anything BUT this is KINDAAA cool
void output_the_image(image_information* img, void* payload, void (output_fromPixel)(image_information*, size_t, size_t, void*) ) {
    for (size_t _y = 0; _y < img->height; _y++) {
        for (size_t _x = 0; _x < img->width; _x++) {
            output_fromPixel(img, _x, _y, payload);
        }
        printf("\n");
    }

    printf("\x1b[0m");
}

void print_color_char_fromimg(image_information *img, size_t x, size_t y, void *payload) {
    double *brighten_sobel = (double *)payload;
    double brighten_amount = brighten_sobel[0];
    double sobel_threshhold = brighten_sobel[1];
    double *pixel = get_pixel(img, x, y);
    double r = pixel[0];
    double g = pixel[1];
    double b = pixel[2];
    brighten(&r, &g, &b, brighten_amount);
    char character_to_use;
    double sobel_x, sobel_y;
    if (x == 0 || x == img->width - 1 || y == 0 || y == img->height - 1) {
        sobel_x = 0;
        sobel_y = 0;
    }
    else
    {
        sobel_x = get_sobel_x(img, x, y);
        sobel_y = get_sobel_y(img, x, y);
    }
    double sobel_magnitude = sqrt(sobel_x * sobel_x + sobel_y * sobel_y);
    double sobel_angle = atan2(sobel_y, sobel_x) * 180.0 / 3.14159;
    if (sobel_magnitude > sobel_threshhold) {
        character_to_use = get_sobel_angle_char(sobel_angle);
    }
    else {
        character_to_use = get_ascii_char(get_grayscale_from_rgb(r, g, b));
    }
    printf("\x1b[38;2;%d;%d;%dm%c", (int)r, (int)g, (int)b, character_to_use);
}


void print_colorless_char(image_information *img, size_t x, size_t y, void *payload) {
    double *brighten_sobel = (double *)payload;
    double brighten_amount = brighten_sobel[0];
    double sobel_threshhold = brighten_sobel[1];
    double *pixel = get_pixel(img, x, y);
    char character_to_use;
    double sobel_x, sobel_y;
    if (x == 0 || x == img->width - 1 || y == 0 || y == img->height - 1) {
        sobel_x = 0;
        sobel_y = 0;
    }
    else
    {
        sobel_x = get_sobel_x(img, x, y);
        sobel_y = get_sobel_y(img, x, y);
    }
    double sobel_magnitude = sqrt(sobel_x * sobel_x + sobel_y * sobel_y);
    double sobel_angle = atan2(sobel_y, sobel_x) * 180.0 / 3.14159;
    if (sobel_magnitude > sobel_threshhold) {
        character_to_use = get_sobel_angle_char(sobel_angle);
    }
    else {
        character_to_use = get_ascii_char((*pixel * brighten_amount)/255.0);
    }
    printf("%c", character_to_use);
}


void output_to_file(image_information* img, double brighten_amount, double sobel_threshhold, const char* out_filename) {
    convert_to_grayscale(img);
    FILE * fptr;

    fptr = fopen(out_filename, "w");
    if (!fptr) {
        printf("unable to open file"); 
        return;}
    fprintf(fptr, "Some text");

    fclose(fptr);
    if (String_EndsWith(out_filename, ".rtf")) {
        printf("RTF");
    } else {
        printf("UNRTF");
    }

}



void out_image(image_information* img, double brighten_amount, double sobel_threshhold, int color_option, const char* out_filename) {
    
    double payload[] = {brighten_amount, sobel_threshhold};
    void (*output_function_ptr)(image_information*, size_t, size_t, void*) = color_option == 1 ? &print_color_char_fromimg : &print_colorless_char;
    //  no colors
    if (color_option == 0) {
        convert_to_grayscale(img);
        
    } 
    if (out_filename != NULL) {
        output_to_file(img, brighten_amount, sobel_threshhold, out_filename);
        return;
    }

    output_the_image(img, (void*) payload, output_function_ptr);
}