#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CMAX 600

#include <errno.h>
#define DIE(assertion, call_description)				\
	do {												\
		if (assertion) {								\
			fprintf(stderr, "(%s, %d): ",				\
					__FILE__, __LINE__);				\
			perror(call_description);					\
			exit(errno);								\
		}												\
	} while (0)


typedef struct dll_node {
	size_t address;
	int id;
	int is_written;
	void *data;
	struct dll_node *prev, *next;
} dll_node_t;

typedef struct {
	int size;
	int nr_o_nodes;
	dll_node_t *head, *tail;
} dll_t;

typedef struct {
	int nr_l;
	int dim;
	int nbytes;
	int rec_type;
	int free_blocks;
	int frag;
	int free_mem;
	int total_mem;
	dll_t **list;
} sfl_t;

typedef struct {
	int nr_l;
	int is_used;
	int dim;
	int mall_calls;
	int free_calls;
	size_t total_mem_used, adr_min;
	int used_blocks;
	dll_t **list;
} ium_t;

//create the doubly-linked lists which store the initial, empty blocks
dll_t **create_lists(size_t adr, int n_list, int nbytes);

sfl_t *create_free_mem(size_t adr, int n_list, int nbytes, int rec_type);

ium_t *init_mem(void);

void dll_add_block(dll_t *list, size_t addr, int id);

dll_node_t *ll_remove_nth_node(dll_t **list, unsigned int n);

//swaps two lists
void dll_swap(dll_t **a, dll_t **b);

//finds and returns the index of the list that has the size "size"
//if it returns -1 it means that the list does not exist and the arrary
//of lists needs to be realloc'd
int dle(dll_t **arr, int nr_list, int size);

//checks if there is a list that has a size of 'n_bytes'
//and if there isn't it creates it (reallocs the array of lists if necessary)
void IUM_check_do_realloc(ium_t **mem, int n_bytes);

//checks if there is a list that has a size of 'n_bytes'
//and if there isn't it creates it (reallocs the array of lists if necessary)
void SEG_check_do_realloc(sfl_t **mem, int n_bytes);

//returns the index of the list that has >= n_bytes
int do_i_have_mem(sfl_t **fm, int n_bytes);

void my_malloc(sfl_t **free_mem, ium_t **mem, int n_bytes);


#endif