#include "../include/output_image.h"
#include "../include/image.h"
#include "../include/cla_parse.h"

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
                fprintf(outstream ,"\x1b[38;2;%03d;%03d;%03dm%c", (int) r, (int) g, (int) b, character_to_use);
            }
        }
        fprintf(outstream, "\n");
    }

    fprintf(outstream, "\x1b[0m");
    // ansi escape color code causes the next lines on windows powershell in non admin mode to continue using the last used color
    // not a super big deal
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
        color_option == 1 ? printf("You may need to use a terminal viewer to see color -> use cat %s to view in your terminal or use the -r flag to view the output \n", out_filename) : printf("\n");
    }
    
}

long get_file_size(FILE *fp) {
    long size;
    // Go to the end of the file
    fseek(fp, 0L, SEEK_END);
    // Get the current position, which is the file size
    size = ftell(fp);
    
    // Go back to the beginning for fread
    fseek(fp, 0L, SEEK_SET);
    return size;
}

char* read_file(const char* in_filepath) {
    FILE* file;
    char *buffer = NULL;
    long file_size;
    file = fopen(in_filepath, "rb");
    if (!file) {
        return NULL;
    } 
    
    file_size = get_file_size(file);
    if (file_size < 0) {
        fprintf(stderr, "Failed to read to file\n");
        fclose(file);
        return NULL;
    }

    buffer = (char*) malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    long result = fread(buffer, 1, file_size, file);
    if (result != file_size) {
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(file);

    return buffer;

}

void print_ascii_file(const char* filepath) {
    char* content = read_file(filepath);

    if (content == NULL) {
        fprintf(stderr, "File Read failed\n");
        return;
    }

    printf("%s",content);
    free(content);
}

void produce_output(args_list* args) {
    image_information* img = load_image(args->file_path);
    if (!img) {
        return;
    }
    resize_image(img, args->max_width, args->max_height, args->character_ratio);

    out_image(img, args->brighten_amount, args->edge_sobel_threshold, args->color_option, args->output_file_path);
    
    rc_free_image_info(img);
    
}