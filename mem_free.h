#ifndef MEM_FREE_H
#define MEM_FREE_H
#include "mem_alloc.h"
#include "read_write.h"
#include "dump.h"

//free list
void dll_free(dll_t **pp_list);

void destroy_heap(sfl_t **free_mem, ium_t **mem);

//reconstruction type == 1
//finds block that originated from the same block and glues them
void my_free_1(sfl_t **fm, ium_t **mem, size_t adr, int i);

void my_free(sfl_t **fm, ium_t **mem, size_t address);

#endif