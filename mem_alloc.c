#include <stdio.h>
#include <limits.h>
#include "mem_alloc.h"

//create the doubly-linked lists which store the initial, empty blocks
dll_t **create_lists(size_t adr, int n_list, int nbytes)
{
	int p = 8, id = 1;
	dll_t **arr = (dll_t **)malloc(n_list * sizeof(dll_t *));
	DIE(!arr, "failed to create array of lists!\n");
	for (int i = 0; i < n_list; ++i) {
		arr[i] = (dll_t *)malloc(sizeof(dll_t));
		if (!arr[i]) {
			fprintf(stderr, "malloc() dll_t failed in create_lists()\n");
			for (int j = i - 1; j >= 0; --j)
				free(arr[j]);
			free(arr);
		}
		arr[i]->tail = NULL;
		arr[i]->head = NULL;
		arr[i]->nr_o_nodes = 0;
		arr[i]->size = p;
		for (int j = 0; j < nbytes / p; ++j) {
			dll_node_t *new_node = malloc(sizeof(dll_node_t));
			DIE(!new_node, "malloc() dll_node_t failed in create_lists()\n");
			new_node->address = adr;
			new_node->id = id++;
			new_node->is_written = 0;
			adr += p;
			arr[i]->nr_o_nodes++;
			new_node->next = arr[i]->head;
			if (j != 0)
				arr[i]->head->prev = new_node;
			new_node->prev = NULL;
			arr[i]->head = new_node;
			if (j == 0)
				arr[i]->tail = new_node;
		}
		p *= 2;
	}
	return arr;
}

sfl_t *create_free_mem(size_t adr, int n_list, int nbytes, int rec_type)
{
	sfl_t *free_mem = malloc(sizeof(sfl_t));
	DIE(!free_mem, "malloc() failed!\n");
	free_mem->nr_l = n_list;
	free_mem->dim = n_list;
	free_mem->nbytes = nbytes;
	free_mem->rec_type = rec_type;
	free_mem->frag = 0;
	free_mem->list = create_lists(adr, n_list, nbytes);
	free_mem->free_mem = n_list * nbytes;
	free_mem->total_mem = n_list * nbytes;
	int nr_blocks = 0;
	for (int i = 0, p = 8; i < n_list; ++i, p *= 2)
		nr_blocks += nbytes / p;
	free_mem->free_blocks = nr_blocks;
	return free_mem;
}

ium_t *init_mem(void)
{
	ium_t *mem;
	mem = (ium_t *)malloc(sizeof(ium_t));
	DIE(!mem, "malloc() failed\n");
	mem->list = (dll_t **)malloc(1 * sizeof(dll_t *));
	DIE(!mem->list, "malloc() failed\n");
	mem->nr_l = 0;
	mem->dim = 1;
	mem->is_used = 0;
	mem->mall_calls = 0;
	mem->total_mem_used = 0;
	mem->free_calls = 0;
	mem->used_blocks = 0;
	mem->adr_min = ULONG_MAX;
	return mem;
}

void dll_add_block(dll_t *list, size_t addr, int id)
{
	dll_node_t *new_node = (dll_node_t *)malloc(sizeof(dll_node_t));
	DIE(!new_node, "malloc() failed dll_add_nth_node()\n");
	new_node->address = addr;
	new_node->id = id;
	if (list->nr_o_nodes == 0) {
		new_node->next = NULL;
		new_node->prev = NULL;
		list->head = new_node;
		list->tail = new_node;
	} else {
		dll_node_t *aux = list->tail;
		//if the address is smaller than the tail's address
		//insert node before the tail
		if (new_node->address < aux->address) {
			new_node->next = NULL;
			new_node->prev = aux;
			list->tail = new_node;
		}
		while (aux->prev && aux->address < new_node->address)
			aux = aux->prev;
		if (aux->address < new_node->address) {
			new_node->next = aux;
			aux->prev = new_node;
			new_node->prev = NULL;
			list->head = new_node;
		} else {
			new_node->next = aux->next;
			new_node->prev = aux;
			if (aux->next)
				new_node->next->prev = new_node;
			aux->next = new_node;
		}
	}
	new_node->is_written = 0;
	list->nr_o_nodes++;
}

dll_node_t *ll_remove_nth_node(dll_t **list, unsigned int n)
{
	dll_node_t *prev, *curr;

	if (!(*list) || !(*list)->head)
		return NULL;
	curr = (*list)->head;
	prev = NULL;
	if ((*list)->nr_o_nodes == 1) {
		(*list)->head = NULL;
		(*list)->tail = NULL;
	} else {
		/* n >= list->size - 1 free last node */
		if ((int)n > (*list)->nr_o_nodes - 1)
			n = (*list)->nr_o_nodes - 1;
		while (n > 0) {
			prev = curr;
			curr = curr->next;
			--n;
		}
		if (!prev) {
			/* n == 0. */
			(*list)->head = curr->next;
		} else {
			prev->next = curr->next;
			(*list)->tail = prev;
		}
	}
	(*list)->nr_o_nodes--;
	return curr;
}

//swaps two lists
void dll_swap(dll_t **a, dll_t **b)
{
	dll_t *aux = *a;
	*a = *b;
	*b = aux;
}

//finds and returns the index of the list that has the size "size"
//if it returns -1 it means that the list does not exist and the arrary
//of lists needs to be realloc'd
int dle(dll_t **arr, int nr_list, int size)
{
	int ok = -1;
	for (int i = 0; i < nr_list; ++i) {
		if (arr[i]->size == size)
			ok = i + 1;
	}
	return ok;
}

//checks if there is a list that has a size of 'n_bytes'
//and if there isn't it creates it (reallocs the array of lists if necessary)
void IUM_check_do_realloc(ium_t **mem, int n_bytes)
{
	if (dle((*mem)->list, (*mem)->nr_l, n_bytes) == -1) {
		if ((*mem)->nr_l == (*mem)->dim) {
			(*mem)->list = realloc((*mem)->list, (*mem)->dim *
									2 * sizeof(dll_t *));
			(*mem)->dim = 2 * (*mem)->dim;
			int i = 0;
			for (i = 0; i < (*mem)->nr_l; ++i)
				if (n_bytes < (*mem)->list[i]->size)
					break;
			if (i >= (*mem)->nr_l)
				i = (*mem)->nr_l;
			//move lists to the right
			(*mem)->list[(*mem)->nr_l] = (dll_t *)malloc(sizeof(dll_t));
			DIE(!(*mem)->list[(*mem)->nr_l], "Malloc() failed\n");
			for (int j = (*mem)->nr_l - 1; j >= i ; --j)
				dll_swap(&(*mem)->list[j + 1], &(*mem)->list[j]);
			(*mem)->list[i]->size = n_bytes;
			(*mem)->list[i]->nr_o_nodes = 0;
			(*mem)->nr_l++;
		} else {
			int i = 0;
			for (i = 0; i < (*mem)->nr_l; ++i)
				if (n_bytes < (*mem)->list[i]->size)
					break;
			if (i >= (*mem)->nr_l)
				i = (*mem)->nr_l;
			//move lists to the right
			(*mem)->list[(*mem)->nr_l] = (dll_t *)malloc(sizeof(dll_t));
			DIE(!(*mem)->list[(*mem)->nr_l], "malloc() failed\n");
			for (int j = (*mem)->nr_l - 1; j >= i ; --j)
				dll_swap(&(*mem)->list[j + 1], &(*mem)->list[j]);
			(*mem)->list[i]->size = n_bytes;
			(*mem)->list[i]->nr_o_nodes = 0;
			(*mem)->nr_l++;
		}
	}
}

//checks if there is a list that has a size of 'n_bytes'
//and if there isn't it creates it (reallocs the array of lists if necessary)
void SEG_check_do_realloc(sfl_t **mem, int n_bytes)
{
	if (dle((*mem)->list, (*mem)->nr_l, n_bytes) == -1) {
		if ((*mem)->nr_l == (*mem)->dim) {
			(*mem)->list = realloc((*mem)->list, (*mem)->dim * 2 *
									 sizeof(dll_t *));
			(*mem)->dim = 2 * (*mem)->dim;
			int i = 0;
			for (i = 0; i < (*mem)->nr_l; ++i)
				if (n_bytes < (*mem)->list[i]->size)
					break;
			if (i >= (*mem)->nr_l)
				i = (*mem)->nr_l;
			//move lists to the right
			(*mem)->list[(*mem)->nr_l] = malloc(sizeof(dll_t));
			DIE(!(*mem)->list[(*mem)->nr_l], "malloc() failed\n");
			for (int j = (*mem)->nr_l - 1; j >= i ; --j)
				dll_swap(&(*mem)->list[j + 1], &(*mem)->list[j]);
			(*mem)->list[i]->size = n_bytes;
			(*mem)->list[i]->nr_o_nodes = 0;
			(*mem)->nr_l++;
			(*mem)->list[i]->tail = NULL;
			(*mem)->list[i]->head = NULL;
		} else {
			int i = 0;
			for (i = 0; i < (*mem)->nr_l; ++i)
				if (n_bytes < (*mem)->list[i]->size)
					break;
			if (i >= (*mem)->nr_l)
				i = (*mem)->nr_l;
			//move lists to the right
			(*mem)->list[(*mem)->nr_l] = malloc(sizeof(dll_t));
			DIE(!(*mem)->list[(*mem)->nr_l], "Malloc() failed\n");
			for (int j = (*mem)->nr_l - 1; j >= i ; --j)
				dll_swap(&(*mem)->list[j + 1], &(*mem)->list[j]);
			(*mem)->list[i]->size = n_bytes;
			(*mem)->list[i]->nr_o_nodes = 0;
			(*mem)->nr_l++;
			(*mem)->list[i]->tail = NULL;
			(*mem)->list[i]->head = NULL;
		}
	}
}

//returns the index of the list that has >= n_bytes
int do_i_have_mem(sfl_t **fm, int n_bytes)
{
	int i;
	for (i = 0; i < (*fm)->nr_l; ++i)
		if ((*fm)->list[i]->size >= n_bytes && (*fm)->list[i]->nr_o_nodes > 0)
			return i;
	return i;
}

void my_malloc(sfl_t **free_mem, ium_t **mem, int n_bytes)
{
	//check if there is enough memory left
	int i = do_i_have_mem(free_mem, n_bytes);
	if (i >= (*free_mem)->nr_l) {
		printf("Out of memory\n");
	} else {
		//create list of n_bytes size if needed
		IUM_check_do_realloc(mem, n_bytes);
		int ok = dle((*mem)->list, (*mem)->nr_l, n_bytes) - 1;
		(*mem)->mall_calls++;
		dll_node_t *new_block = (*free_mem)->list[i]->tail;
		if ((*free_mem)->list[i]->nr_o_nodes > 1) {
			(*free_mem)->list[i]->tail = (*free_mem)->list[i]->tail->prev;
			(*free_mem)->list[i]->tail->next = NULL;
		} else {
			(*free_mem)->list[i]->tail = NULL;
		}
		//update variables
		(*free_mem)->free_blocks--;
		(*free_mem)->free_mem -= (*free_mem)->list[i]->size;
		(*mem)->total_mem_used += n_bytes;
		(*mem)->used_blocks++;
		(*free_mem)->list[i]->nr_o_nodes--;
		if (new_block->address < (*mem)->adr_min)
			(*mem)->adr_min = new_block->address;
		if ((*free_mem)->list[i]->size == n_bytes) {
			//add entire block
			dll_add_block((*mem)->list[ok], new_block->address, new_block->id);
		} else {
			//fragmentation is needed
			(*free_mem)->frag++;
			dll_node_t *old = (dll_node_t *)malloc(sizeof(dll_node_t));
			DIE(!old, "malloc() failed\n");
			old->address = new_block->address + n_bytes;
			int new_size = (*free_mem)->list[i]->size - n_bytes;
			(*free_mem)->free_mem += new_size;
			SEG_check_do_realloc(free_mem, new_size);
			int j = dle((*free_mem)->list, (*free_mem)->nr_l, new_size) - 1;
			dll_add_block((*mem)->list[ok], new_block->address, new_block->id);
			dll_add_block((*free_mem)->list[j], old->address, new_block->id);
			(*free_mem)->free_blocks++;
			free(old);
		}
		free(new_block);
	}
}