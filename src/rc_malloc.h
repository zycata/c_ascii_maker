

#ifndef RC_MALLOC
#define RC_MALLOC

void * rc_malloc(unsigned long long  nbytes);

void rc_keep_ref(void* ptr);

void rc_free_ref(void* ptr);

unsigned long rc_get_refs(void* ptr);




#endif