#ifndef RW_H
#define RW_H
#include "mem_alloc.h"
#include "mem_free.h"
#include "dump.h"

int is_allocated(ium_t **mem, size_t adr);

//returns 1 if there is enough memory to write 'n_bytes' bytes
//or -1 if there isn't
int is_enough_all(ium_t **mem, size_t adr, int nr_bytes);

void write(ium_t **mem, size_t adr, char s[CMAX], int nr_bytes, int i);

void read(ium_t **mem, size_t adr, int nr_bytes);

#endif