#include <stdio.h>
#include <limits.h>
#include "mem_alloc.h"

//free list
void dll_free(dll_t **pp_list)
{
	dll_node_t *curr_node;
	while ((*pp_list)->nr_o_nodes > 0) {
		curr_node = ll_remove_nth_node(&(*pp_list), 0);
		if (curr_node->is_written > 0)  {
			//printf("STERG data\n");
			free(curr_node->data);
			curr_node->data = NULL;
		}
		free(curr_node);
		curr_node = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}

void destroy_heap(sfl_t **free_mem, ium_t **mem)
{
	for (int i = 0; i < (*free_mem)->nr_l; ++i)
		dll_free(&(*free_mem)->list[i]);
	free((*free_mem)->list);
	free((*free_mem));
	for (int i = 0; i < (*mem)->nr_l; ++i)
		dll_free(&(*mem)->list[i]);
	free((*mem)->list);
	free((*mem));
}

//reconstruction type == 1
//finds block that originated from the same block and glues them
void my_free_1(sfl_t **fm, ium_t **mem, size_t adr, int i)
{
	dll_node_t *old = (*mem)->list[i]->tail;
	while (old->address != adr)
		old = old->prev;
	int size = (*mem)->list[i]->size;
	for (int j = 0; j < (*fm)->nr_l; ++j) {
		dll_node_t *curr = (*fm)->list[j]->tail;
		int p = (*fm)->list[j]->nr_o_nodes;
		int ok = 0;
		while (curr && (*fm)->list[j]->nr_o_nodes > 0) {
			if (curr->id == old->id) {
				if (curr->address == old->address + size ||
					old->address == curr->address + (*fm)->list[j]->size) {
					size += (*fm)->list[j]->size;
					if (curr->address < old->address)
						old->address = curr->address;
					curr = curr->prev;
					dll_node_t *dest;
					dest = ll_remove_nth_node(&(*fm)->list[j], p - 1);
					if (dest->is_written)
						free(dest->data);
					free(dest);
					dest = NULL;
					ok = 1;
					(*fm)->free_blocks--;
				}
				if (!ok)
					curr = curr->prev;
			} else {
				curr = curr->prev;
				p--;
			}
		}
		if (ok)
			j = -1;
	}
	SEG_check_do_realloc(fm, size);
	int j = dle((*fm)->list, (*fm)->nr_l, size) - 1;
	dll_add_block((*fm)->list[j], old->address, old->id);
	(*fm)->free_blocks++;
	//free old
	if ((*mem)->list[i]->nr_o_nodes == 1) {
		(*mem)->list[i]->head = NULL;
		(*mem)->list[i]->tail = NULL;
	} else {
		if (old->next) {
			old->next->prev = old->prev;
		} else {
			old->prev->next = NULL;
			(*mem)->list[i]->tail = old->prev;
		}
		if (old->prev) {
			old->prev->next = old->next;
		} else {
			old->next->prev = NULL;
			(*mem)->list[i]->head = old->next;
		}
	}
	//updates the lowest address of the alloc'd blocks
	if (old->address == (*mem)->adr_min) {
		size_t min_adr = ULONG_MAX;
		for (int i = 0; i < (*mem)->nr_l; ++i) {
			dll_node_t *new_node;
			new_node = (*mem)->list[i]->tail;
			while (new_node) {
				if (new_node->address > (*mem)->adr_min &&
					new_node->address < min_adr)
					min_adr = new_node->address;
				new_node = new_node->prev;
			}
		}
		(*mem)->adr_min = min_adr;
	}
	if (old->is_written)
		free(old->data);
	free(old);
	(*mem)->list[i]->nr_o_nodes--;
	(*mem)->used_blocks--;
	(*fm)->free_mem += (*mem)->list[i]->size;
	(*mem)->total_mem_used -= (*mem)->list[i]->size;
}

void my_free(sfl_t **fm, ium_t **mem, size_t address)
{
	//check if there is an alloc'd block at the address
	int i = 0, ok = 0, p;
	dll_node_t *curr;
	for (i = 0; i < (*mem)->nr_l && !ok; ++i) {
		curr = (*mem)->list[i]->tail;
		ok = 0;
		p = 0;
		while (curr) {
			if (curr->address == address) {
				ok = 1;
				break;
			}
			curr = curr->prev;
			p++;
		}
	}
	i--;
	if (!ok) {
		printf("Invalid free\n");
		return;
	} else if ((*fm)->rec_type == 0) {
		(*mem)->free_calls++;
		int block_size = (*mem)->list[i]->size;
		//check if there is coresponding size list in free_mem
		//and create it if not
		SEG_check_do_realloc(fm, block_size);
		int j = dle((*fm)->list, (*fm)->nr_l, block_size) - 1;
		dll_add_block((*fm)->list[j], address, curr->id);
		//remove block
		dll_node_t *old = (*mem)->list[i]->tail;
		while (p > 0) {
			old = old->prev;
			p--;
		}
		if ((*mem)->list[i]->nr_o_nodes == 1) {
			(*mem)->list[i]->head = NULL;
			(*mem)->list[i]->tail = NULL;
		} else {
			if (old->next) {
				old->next->prev = old->prev;
			} else {
				old->prev->next = NULL;
				(*mem)->list[i]->tail = old->prev;
			}
			if (old->prev) {
				old->prev->next = old->next;
			} else {
				old->next->prev = NULL;
				(*mem)->list[i]->head = old->next;
			}
		}
		//update variables
		(*mem)->list[i]->nr_o_nodes--;
		(*fm)->free_blocks++;
		(*fm)->free_mem += (*mem)->list[i]->size;
		(*mem)->total_mem_used -= (*mem)->list[i]->size;
		(*mem)->used_blocks--;
		if (old->address == (*mem)->adr_min) {
			size_t min_adr = ULONG_MAX;
			for (int i = 0; i < (*mem)->nr_l; ++i) {
				dll_node_t *new_node;
				new_node = (*mem)->list[i]->tail;
				while (new_node) {
					if (new_node->address > (*mem)->adr_min &&
						new_node->address < min_adr)
						min_adr = new_node->address;
					new_node = new_node->prev;
				}
			}
			(*mem)->adr_min = min_adr;
		}
		if (old->is_written)
			free(old->data);
		free(old);
	} else if ((*fm)->rec_type == 1) {
		(*mem)->free_calls++;
		my_free_1(fm, mem, address, i);
	}
}