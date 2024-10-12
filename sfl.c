//Brandibur Tudor 313CAa, SDA tema 1, 2023-2024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define CMAX 600


#include "mem_alloc.h"
#include "mem_free.h"
#include "read_write.h"
#include "dump.h"

int main(void)
{
	char com[CMAX];
	sfl_t *free_mem;
	ium_t *mem = init_mem();
	int n_list, nbytes, rec_type;
	int k = 1;
	while (k) {
		scanf("%s", com);
		if (!strcmp(com, "INIT_HEAP")) {
			size_t adr;
			scanf("%lx%d%d%d", &adr, &n_list, &nbytes, &rec_type);
			free_mem = create_free_mem(adr, n_list, nbytes, rec_type);
		} else if (!strcmp(com, "MALLOC")) {
			scanf("%d", &nbytes);
			my_malloc(&free_mem, &mem, nbytes);
		} else if (!strcmp(com, "FREE")) {
			size_t address;
			scanf("%lx", &address);
			my_free(&free_mem, &mem, address);
		} else if (!strcmp(com, "READ")) {
			size_t adr;
			int nr_bytes;
			scanf("%lx%d", &adr, &nr_bytes);
			int i = is_allocated(&mem, adr);
			if (i >= 0 && is_enough_all(&mem, adr, nr_bytes) == 1) {
				read(&mem, adr, nr_bytes);
			} else {
				printf("Segmentation fault (core dumped)\n");
				dump_my_memory(free_mem, mem);
				destroy_heap(&free_mem, &mem);
				return 0;
			}
		} else if (!strcmp(com, "WRITE")) {
			size_t adr;
			char s[CMAX], t[CMAX];
			unsigned int nr_b;
			scanf("%lx", &adr);
			fgets(s, CMAX, stdin);
			char *p = strtok(s, "\"");
			p = strtok(NULL, "\"");
			strcpy(t, p);
			p = strtok(NULL, " ");
			nr_b = atoi(p);
			if (nr_b >= strlen(t))
				nr_b = strlen(t);
			int i = is_allocated(&mem, adr);
			if (i >= 0 && is_enough_all(&mem, adr, nr_b) == 1) {
				write(&mem, adr, t, nr_b, i);
			} else {
				printf("Segmentation fault (core dumped)\n");
				dump_my_memory(free_mem, mem);
				destroy_heap(&free_mem, &mem);
				return 0;
			}
		} else if (!strcmp(com, "DUMP_MEMORY")) {
			dump_my_memory(free_mem, mem);
		} else if (!strcmp(com, "DESTROY_HEAP")) {
			destroy_heap(&free_mem, &mem);
			k = 0;
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
