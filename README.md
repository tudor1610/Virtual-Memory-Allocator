**Copyright Tudor Brandibur 2023-2024**


## Virtual Memory Allocator

### Description:

* This assignment is a simulator for allocating and freeing a computer's memory. The program uses two structures: **`sfl_t`**, which stores information about free memory, and **`ium_t`**, which stores information about memory allocated within the program. **`sfl_t`** uses an array of doubly linked lists, where the free memory blocks are stored. After calling the **`my_malloc()`** function, the blocks are moved to the array of doubly linked lists within the **`ium_t`** structure.

* Additional explanations for certain parts of the assignment that you might think are not clear enough just by following the code:

	* In the **`print_all_arr()`** function, to display the allocated blocks in ascending order, I print the node with the smallest address, then traverse the array of lists to find the next smallest address, and so on.
	* The **`IUM_check_do_realloc()`** function checks if there is a list that has the same size as the number of bytes received as a parameter. If such a list does not exist, it creates one after checking if the array of doubly linked lists needs to be resized. The new list is created at the last position in the array and then moved to the correct position (the lists are sorted in ascending order by their size) using the **`dll_swap()`** function.
	* The **`my_free_1()`** function is called in the case of type 1 reconstruction (the bonus). The array of lists is traversed to check if the block received as a parameter, via its address, can be merged with other blocks. Merging is done only if the blocks have consecutive addresses and the same ID (the ID indicates the block from which they originated). If two blocks are merged, the search restarts from the beginning of the array of lists.