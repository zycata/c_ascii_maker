

#include "../include/rc_malloc.h"


// why the fuck is null needed here bruhify
#include <stdlib.h>
// extra padding for rc_malloc short for reference count malloc
#define REF_COUNT_TYPE unsigned long
#define RC_EXTRA_SIZE sizeof(REF_COUNT_TYPE)
// im literally doing this project just to use rc_malloc fuck cs213

void* rc_malloc(unsigned long long nbytes) {
    char *ref_count_start = malloc(nbytes + RC_EXTRA_SIZE);

    // failed to allocate the ram
    if (!ref_count_start) {
        return NULL;
    }
    REF_COUNT_TYPE *ref_count = (REF_COUNT_TYPE *)ref_count_start;


    *ref_count = 1;

    return ref_count_start + RC_EXTRA_SIZE;
}

void rc_keep_ref(void* ptr) {
    REF_COUNT_TYPE *ref_count = (REF_COUNT_TYPE *)(((char*)ptr) - RC_EXTRA_SIZE);
    (*ref_count)++;
}

void rc_free_ref(void* ptr) {
    // is this peak engineering this line below? 
    if (!ptr) return;
    char *ref_count_start = ((char*)ptr) - RC_EXTRA_SIZE;
    REF_COUNT_TYPE *ref_count = (REF_COUNT_TYPE *)ref_count_start;

    (*ref_count)--;

    if (*ref_count == 0) {
        free(ref_count_start);
    }
}

unsigned long rc_get_refs(void* ptr) {
    REF_COUNT_TYPE *ref_count = (REF_COUNT_TYPE *)(((char*)ptr) - RC_EXTRA_SIZE);
    
    return *ref_count;
}





