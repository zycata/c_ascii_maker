
#ifndef CLAPARSE
#define CLAPARSE

typedef struct {
    char* file_path;
    size_t max_width;
    size_t max_height;
    double edge_sobel_threshold;
    int color_option;


    double character_ratio; 
    double brighten_amount;

    double contrast_change; // todo
    char* output_file_path;
    // will probably need to be freed heap mmeory go brr
    char* value_chars;
} args_list;




void test_system(void);

int get_terminal_size(size_t* width, size_t* height);


int parse_arguments(int argc, char* argv[], args_list* arguments);

void print_arguments(const args_list* arguments);


#endif
