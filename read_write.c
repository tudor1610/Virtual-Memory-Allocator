#include <stdio.h>
#include "mem_alloc.h"

int is_allocated(ium_t **mem, size_t adr)
{
	int i = 0, ok = 0, p;
	dll_node_t *curr;
	for (i = 0; i < (*mem)->nr_l && !ok; ++i) {
		curr = (*mem)->list[i]->tail;
		ok = 0;
		p = 0;
		while (curr) {
			if (curr->address == adr) {
				ok = 1;
				break;
			}
			//daca nu exista adresa
			if (curr->prev) {
				curr = curr->prev;
				p++;
			} else {
				break;
			}
		}
	}
	if (ok) {
		i--;
		return i;
	} else {
		return -1;
	}
}

//returns 1 if there is enough memory to write 'n_bytes' bytes
//or -1 if there isn't
int is_enough_all(ium_t **mem, size_t adr, int nr_bytes)
{
	int i = is_allocated(&(*mem), adr);
	int size = (*mem)->list[i]->size;
	int aux_adr = adr + size;
	while (size < nr_bytes) {
		i = is_allocated(&(*mem), aux_adr);
		if (i >= 0) {
			size += (*mem)->list[i]->size;
			aux_adr += (*mem)->list[i]->size;
		} else {
			break;
		}
	}
	if (size < nr_bytes)
		return -1;
	return 1;
}

void write(ium_t **mem, size_t adr, char s[CMAX], int nr_bytes, int i)
{
	char t[CMAX];
	strcpy(t, s);
	int size = 0;
	int h = i;
	size_t aux_adr = adr;
	do {
		size = (*mem)->list[h]->size;
		adr = aux_adr;
		aux_adr += size;
		dll_node_t *curr = (*mem)->list[h]->tail;
		while (curr) {
			if (curr->address == adr)
				break;
			curr = curr->prev;
		}
		if (size <= nr_bytes) {
			if (!curr->is_written) {
				curr->data = malloc(size + 1);
				DIE(!curr->data, "malloc failed\n");
				curr->is_written++;
			} else {
				free(curr->data);
				curr->data = malloc(size + 1);
				DIE(!curr->data, "malloc() failed\n");
			}
			strncpy(t, s, size);
			t[size] = '\0';
			memcpy(curr->data, t, size + 1);
			nr_bytes -= size;
			strcpy(t, s + size);
			strcpy(s, t);
		} else {
			if (curr->is_written) {
				char l[CMAX];
				strncpy(l, curr->data, size + 1);
				l[size] = '\0';
				strcat(s, l + nr_bytes);
				free(curr->data);
			}
			s[strlen(s)] = '\0';
			curr->data = malloc(strlen(s) + 1);
			DIE(!curr->data, "malloc() failed\n");
			curr->is_written++;
			memcpy(curr->data, s, strlen(s) + 1);
			nr_bytes = -1;
		}
		h = is_allocated(&(*mem), aux_adr);
	} while (nr_bytes > 0);
}

void read(ium_t **mem, size_t adr, int nr_bytes)
{
	int size = 0;
	int h = is_allocated(&(*mem), adr);
	size_t aux_adr = adr;
	do {
		size = (*mem)->list[h]->size;
		adr = aux_adr;
		aux_adr += size;
		dll_node_t *curr = (*mem)->list[h]->tail;
		while (curr) {
			if (curr->address == adr)
				break;
			curr = curr->prev;
		}
		if (size < nr_bytes) {
			char s[CMAX];
			memcpy(s, curr->data, size);
			nr_bytes -= size;
			s[size] = '\0';
			printf("%s", s);
		} else {
			char s[CMAX];
			memcpy(s, curr->data, nr_bytes);
			s[nr_bytes] = '\0';
			printf("%s", s);
			nr_bytes = -1;
		}
		h = is_allocated(&(*mem), aux_adr);
	} while (nr_bytes > 0);
	printf("\n");
}