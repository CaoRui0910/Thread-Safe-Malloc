#include "my_malloc.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//head of the Linked List of free memory regions (freed blocks)
metadata * headOfFreeList = NULL;
//tail of the Linked List of free memory regions (freed blocks)
metadata * tailOfFreeList = NULL;
//entire heap memory (this includes memory used to save metadata)
size_t dataSegmentSize = 0;

void * twoKindsMalloc(size_t size, bool isff){
    if(headOfFreeList != NULL){
        metadata * blockAllocated;
        if(isff){
            blockAllocated = ffFindBlock(size);
        }else{
            blockAllocated = bfFindBlock(size);
        }
        if(blockAllocated == NULL){
            //No suitable freed blocks, need to create space using sbrk()
            //////////////////////////////////?????????////////////
            return (char *)allocNewSpace(size) + sizeof(metadata);
        }else{//////////////////////////////////?????????////////////
            if(blockAllocated->size > size + sizeof(metadata)){
                //need to split free regions if the ideal free region is larger than requested size.
                splitBlock(blockAllocated, size);
            }else{
                //No need to split
                //the remaining free space is too small to keep track of
                //////?
                blockAllocated->size = size;
                removeFromFreeList(blockAllocated);
            }
            //////////////////////////////////?????????////////////
            return (char *)blockAllocated + sizeof(metadata);
        }
    }else{ 
        //When there is no freed blocks: create space using sbrk()
        //////////////////////////////////?????????////////////
        return (char *)allocNewSpace(size) + sizeof(metadata);
    }
}

//Find suitable freed block from freed blocks list using First Fit
//If there is no suitable freed block, return NULL
metadata * ffFindBlock(size_t size){
    metadata * curr = headOfFreeList;
    //////////////////////////////////?????????////////////
    while(curr != NULL && curr->size < size){
        curr = curr->next;
    }
    return curr;
}

//This function is used to remove block from free blocks linked list
void removeFromFreeList(metadata * blockAllocated){
    //when there is only one element in free blocks list:
    if(headOfFreeList == tailOfFreeList){
        headOfFreeList = NULL;
        tailOfFreeList = NULL;
    }else if(headOfFreeList == blockAllocated && headOfFreeList != tailOfFreeList){
        //when there is more than one element and remove from front:
        headOfFreeList = blockAllocated->next;
        headOfFreeList->prev = NULL;
    }else if(tailOfFreeList == blockAllocated && headOfFreeList != tailOfFreeList){
        //when there is more than one element and remove from back:
        tailOfFreeList = blockAllocated->prev;
        tailOfFreeList->next = NULL;
    }else{
        //other removals:
        blockAllocated->prev->next = blockAllocated->next;
        blockAllocated->next->prev = blockAllocated->prev;
    }
    blockAllocated->next = NULL;
    blockAllocated->prev = NULL;
    //Now, blockAllocated is allocated, so it is not freed
    blockAllocated->isFreed = false;
}

//This function is used to create new block and increase heap space using sbrk()
//Output: The new block allocated
metadata * allocNewSpace(size_t size){
    //////////////////////////////////?????????////////////
    //sbrk() returns the previous program break. ans points at the same place with newBlock
    metadata * ans = (metadata *)sbrk(0);
    void * newBlock = sbrk(size + sizeof(metadata));
    //sbrk(): On error, (void *) -1 is returned
    //check whether sbrk failed or not:
    if(newBlock == (void *) -1){
        return NULL;
    }
    ans->size = size;
    ans->isFreed = false;
    ans->next = NULL;
    ans->prev = NULL;
    //For compute data segment size
    dataSegmentSize += size + sizeof(metadata);
    return ans;
}

//This function is used to split free blocks if the ideal free block is larger than requested size.
void splitBlock(metadata * blockAllocated, size_t size){
    //////////////////////////////////?????????////////////
    metadata * newBlock = (metadata *)((char *)blockAllocated + sizeof(metadata) + size);
    newBlock->size = blockAllocated->size - sizeof(metadata) - size ;
    newBlock->next = blockAllocated->next;
    newBlock->prev = blockAllocated->prev;
    newBlock->isFreed = true;
    if(headOfFreeList == tailOfFreeList){
        headOfFreeList = newBlock;
        tailOfFreeList = newBlock;
    }else if(headOfFreeList == blockAllocated && headOfFreeList != tailOfFreeList){
        headOfFreeList = newBlock;
        headOfFreeList->next->prev = newBlock;
    }else if(tailOfFreeList == blockAllocated && headOfFreeList != tailOfFreeList){
        tailOfFreeList = newBlock;
        tailOfFreeList->prev->next = newBlock;
    }else{
        blockAllocated->next->prev = newBlock;
        blockAllocated->prev->next = newBlock;
    }
    blockAllocated->next = NULL;
    blockAllocated->prev = NULL;
    blockAllocated->size = size;
    blockAllocated->isFreed = false;
}


//Find suitable freed block from freed blocks list using Best Fit
//If there is no suitable freed block, return NULL
metadata * bfFindBlock(size_t size){
    metadata * ans = NULL;
    metadata * curr = headOfFreeList;
    //////////////////////////////////?????????////////////
    while(curr != NULL){
        if(curr->size >= size){
            if(ans == NULL || ans->size > curr->size){
                ans = curr;
            }
            if (curr->size == size){
                ans = curr;
                break;
            }
        }
        curr = curr->next;
    }
    return ans;
}

void ff_free(void *ptr){
    //////////////////////////////////?????????////////////
    //avoid ptr is NULL. free() does nothing on NULL.
    if(ptr == NULL){
        return;
    }
    metadata * blockFreed = (metadata *)((char *)ptr - sizeof(metadata));
    //avoid twice free
    //if(blockFreed->isFreed == false){
    addToFreeList(blockFreed);
    mergeFreedBlocks(blockFreed);
    //}
}

//This function is used to add block into free blocks linked list
void addToFreeList(metadata * blockFreed){
    blockFreed->isFreed = true;
    if(headOfFreeList == NULL && tailOfFreeList == NULL){
        //When there is no element in free blocks list
        headOfFreeList = blockFreed;
        tailOfFreeList = blockFreed;
        blockFreed->next = NULL;
        blockFreed->prev = NULL;
        //????????????????????????????????????
    }else if(blockFreed < headOfFreeList){
        //When the block is added to the front of the linked list
        blockFreed->next = headOfFreeList;
        blockFreed->prev = NULL;
        headOfFreeList->prev = blockFreed;
        headOfFreeList = blockFreed;
        //????????????????????????????????????
    }else if(blockFreed > tailOfFreeList){
        //When the block is added to the back of the linked list
        blockFreed->next = NULL;
        blockFreed->prev = tailOfFreeList;
        tailOfFreeList->next = blockFreed;
        tailOfFreeList = blockFreed;
    }else{
        //Other additions:
        metadata * curr = headOfFreeList->next;
        //????????????????????????????????????
        while(curr != NULL && curr < blockFreed && blockFreed < tailOfFreeList && blockFreed > headOfFreeList){
            curr = curr->next;
        }
        blockFreed->next = curr;
        blockFreed->prev = curr->prev;
        curr->prev->next = blockFreed;
        curr->prev = blockFreed;
    }
}

//This function is used to merge adjacent freed blocks.
void mergeFreedBlocks(metadata * blockFreed){
    //////////////////////////////////?????????////////////
    //////////////////////////////////?????????////////////
    //If the next block after merging is still free, continue merging until it cannot be merged
    if(blockFreed->next != NULL && (char *)blockFreed + sizeof(metadata) + blockFreed->size == (char *)blockFreed->next){
        blockFreed->size += (sizeof(metadata) + blockFreed->next->size);
        //removeFromFreeList(blockFreed->next);
        metadata * nextBlock = blockFreed->next;
        if(nextBlock == tailOfFreeList){
            tailOfFreeList = nextBlock->prev;
            tailOfFreeList->next = NULL;
        }else{
            nextBlock->next->prev = nextBlock->prev;
            blockFreed->next = nextBlock->next;
        }
        nextBlock->next = NULL;
        nextBlock->prev = NULL;
        nextBlock->isFreed = false;
        //TODO/////////////
        
    }

    //If the previous block after merging is still free, continue merging until it cannot be merged
    if(blockFreed->prev != NULL && (char *)blockFreed - sizeof(metadata) - blockFreed->prev->size == (char *)blockFreed->prev){
        blockFreed->prev->size += (sizeof(metadata) + blockFreed->size);
        //removeFromFreeList(blockFreed);
        //TODO
        if(blockFreed == tailOfFreeList){
            tailOfFreeList = blockFreed->prev;
            tailOfFreeList->next = NULL;
        }else{
            blockFreed->next->prev = blockFreed->prev;
            blockFreed->prev->next = blockFreed->next;
        }
        blockFreed->next = NULL;
        blockFreed->prev = NULL;
        blockFreed->isFreed = false;
    }
}

void bf_free(void *ptr){
    ff_free(ptr);
}

void * ff_malloc(size_t size){
    return twoKindsMalloc(size, true);
}
void * bf_malloc(size_t size){
    return twoKindsMalloc(size, false);
}

unsigned long get_data_segment_size(){
    return dataSegmentSize;
}

unsigned long get_data_segment_free_space_size(){
    metadata * curr = headOfFreeList;
    size_t freeListSize = 0;
    while(curr != NULL){
        freeListSize += (curr->size + sizeof(metadata));
        curr = curr->next;
    }
    return freeListSize;
}