#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

//metadata is used to describes information about data block in heap
struct metadata_tag {
    size_t size;
    struct metadata_tag * next;
    struct metadata_tag * prev;
    bool isFreed;
};
typedef struct metadata_tag metadata;

void * twoKindsMalloc(size_t size, bool isff);

//Find suitable freed block from freed blocks list using First Fit
metadata * ffFindBlock(size_t size);

//This function is used to remove block from free blocks linked list
void removeFromFreeList(metadata * blockAllocated);

//This function is used to create new block and increase heap space using sbrk()
//Output: The new block allocated
metadata * allocNewSpace(size_t size);

//This function is used to split free blocks if the ideal free block is larger than requested size.
void splitBlock(metadata * blockAllocated, size_t size);

//Find suitable freed block from freed blocks list using Best Fit
//If there is no suitable freed block, return NULL
metadata * bfFindBlock(size_t size);

//This function is used to add block into free blocks linked list
void addToFreeList(metadata * blockFreed);

//This function is used to merge adjacent freed blocks.
void mergeFreedBlocks(metadata * blockFreed);

//Version1: First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void *ptr);

//Version2: Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void *ptr);

//check the malloc() performance with different allocation policies
unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); //in byte
