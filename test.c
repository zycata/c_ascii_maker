

#include <stdio.h>

#define SOBEL_X {-1, 0, 1, -2, 0, 2, -1, 0, 1}
#define SOBEL_X_ROWS 3;
#define SOBEL_Y_COLS 3;

void fun(double* kernel) {
    for (int i = 0; i < 9; i++) {
        printf("%lf\n", kernel[i]);
    }
}

int main() {
    double kernel[] = SOBEL_X;

    
    fun(kernel);
    return 0;
}