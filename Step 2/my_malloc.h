#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

//metadata is used to describes information about data block in heap
struct metadata_tag {
    size_t size;
    struct metadata_tag * next;
    struct metadata_tag * prev;
    bool isFreed;
};
typedef struct metadata_tag metadata;

//void * twoKindsMalloc(size_t size, bool isff);

//Find suitable freed block from freed blocks list using First Fit
//metadata * ffFindBlock(size_t size);

//This function is used to remove block from free blocks linked list
void removeFromFreeList(metadata * blockAllocated, metadata ** headOfFreeList, metadata ** tailOfFreeList);

//This function is used to create new block and increase heap space using sbrk()
//Output: The new block allocated
metadata * allocNewSpace(size_t size, bool issbrkLock);

//This function is used to split free blocks if the ideal free block is larger than requested size.
void splitBlock(metadata * blockAllocated, size_t size, metadata ** headOfFreeList, metadata ** tailOfFreeList);

//Find suitable freed block from freed blocks list using Best Fit
//If there is no suitable freed block, return NULL
metadata * bfFindBlock(size_t size, metadata * headOfFreeList);

//This function is used to add block into free blocks linked list
void addToFreeList(metadata * blockFreed, metadata ** headOfFreeList, metadata ** tailOfFreeList);

//This function is used to merge adjacent freed blocks.
void mergeFreedBlocks(metadata * blockFreed, metadata ** headOfFreeList, metadata ** tailOfFreeList);

//Version1: First Fit malloc/free
//void * ff_malloc(size_t size);
//void ff_free(void *ptr);

//Version2: Best Fit malloc/free
void * bf_malloc(size_t size, metadata ** headOfFreeList, metadata ** tailOfFreeList, bool issbrkLock);
void bf_free(void *ptr, metadata ** headOfFreeList, metadata ** tailOfFreeList);

//check the malloc() performance with different allocation policies
//unsigned long get_data_segment_size(); //in bytes
//unsigned long get_data_segment_free_space_size(); //in byte


void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);
void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);