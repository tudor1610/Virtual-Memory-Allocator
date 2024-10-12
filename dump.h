#ifndef DUMP_H
#define DUMP_H
#include "mem_alloc.h"
#include "mem_free.h"
#include "read_write.h"

void print_arr(dll_t **arr, int n_list);

void print_all_arr(dll_t **arr, int n_list, size_t madr, int nr_nod);

void dump_my_memory(sfl_t *free_mem, ium_t *mem);

#endif