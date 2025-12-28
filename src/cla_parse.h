
#ifndef CLAPARSE
#define CLAPARSE

typedef struct {
    char* file_path;
    size_t max_width;
    size_t max_height;
    double edge_sobel_threshold;
    int color_option;

    double character_ratio; // width/height
    double brighten_amount;

} args_list;




void test_system();

int get_terminal_size(size_t* width, size_t* height);


int parse_arguments(int argc, char* argv[], args_list* arguments);

void print_arguments(const args_list* arguments);


#endif