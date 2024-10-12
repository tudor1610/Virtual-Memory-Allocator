#include <stdio.h>
#include <limits.h>
#include "mem_alloc.h"

void print_arr(dll_t **arr, int n_list)
{
	for (int i = 0; i < n_list; ++i) {
		if (arr[i]->nr_o_nodes > 0) {
			printf("Blocks with %d bytes - %d free block(s) :",
				   arr[i]->size, arr[i]->nr_o_nodes);
			dll_node_t *new_node;
			new_node = arr[i]->tail;
			while (new_node) {
				printf(" 0x%lx", new_node->address);
				new_node = new_node->prev;
			}
			printf("\n");
		}
	}
}

void print_all_arr(dll_t **arr, int n_list, size_t madr, int nr_nod)
{
	printf("Allocated blocks :");
	//look for the lowest address
	size_t min_adr = ULONG_MAX;
	for (int i = 0; i < n_list; ++i) {
		dll_node_t *new_node;
		new_node = arr[i]->tail;
		while (new_node) {
			if (new_node->address == madr)
				printf(" (0x%lx - %d)", new_node->address, arr[i]->size);
			new_node = new_node->prev;
		}
	}
	dll_node_t *pr;
	int k;
	//prints the allocated blocks in order, finding the smallest address,
	//and updating it after every print
	for (int j = 1; j < nr_nod; ++j) {
		for (int i = 0; i < n_list; ++i) {
			dll_node_t *new_node;
			new_node = arr[i]->tail;
			while (new_node) {
				if (new_node->address > madr && new_node->address < min_adr) {
					min_adr = new_node->address;
					pr = new_node;
					k = i;
				}
				new_node = new_node->prev;
			}
		}
		printf(" (0x%lx - %d)", pr->address, arr[k]->size);
		madr = min_adr;
		min_adr = ULONG_MAX;
	}
	printf("\n");
}

void dump_my_memory(sfl_t *free_mem, ium_t *mem)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", free_mem->total_mem);
	printf("Total allocated memory: %ld bytes\n", mem->total_mem_used);
	printf("Total free memory: %d bytes\n", free_mem->free_mem);
	printf("Free blocks: %d\n", free_mem->free_blocks);
	printf("Number of allocated blocks: %d\n", mem->used_blocks);
	printf("Number of malloc calls: %d\n", mem->mall_calls);
	printf("Number of fragmentations: %d\n", free_mem->frag);
	printf("Number of free calls: %d\n", mem->free_calls);
	print_arr(free_mem->list, free_mem->nr_l);
	print_all_arr(mem->list, mem->nr_l, mem->adr_min, mem->used_blocks);
	printf("-----DUMP-----\n");
}