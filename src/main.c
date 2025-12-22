#include <stdio.h>
#include <stdlib.h>

#include "rc_malloc.h"
#include "cla_parse.h"



int main(int argc, char** argv) {

    printf("argc: %d\n", argc);
    for (int i=0; i < argc; i++) {
        printf("%s ", argv[i]);
    }


    int* reals = rc_malloc(sizeof(int)*10);

    printf("\nreference count: %ld \n", rc_get_refs(reals));


    rc_free_ref(reals);

    test_system();

    
}