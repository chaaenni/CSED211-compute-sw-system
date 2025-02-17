/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*****newly defined macros******/
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12) /*Extend heap by this amount (bytes)*/
#define MAX(x,y) (x > y? x: y)

/*pack a size and allocated bit into a word*/
#define PACK(size, alloc) (size | alloc)

/*read and write a word at address p*/
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/*read the size and allocated fields from address p*/
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/*given block ptr bp, compute address of its header and footer*/
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/*given block ptr bp, compute address of next and previous blocks*/
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

static char *heap_listp = NULL;
static char *next = NULL;
/*****head of user defined function*****/
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /*Create the initial empty heap*/
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void*)-1)
		return -1;

	PUT(heap_listp, 0);								/* Alignment padding */
	PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));  /* Prologue header */
	PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));  /* Prologue footer */
	PUT(heap_listp + (3 * WSIZE), PACK(0, 1));      /* Epilogue header */
	heap_listp += (2 * WSIZE);
	next = heap_listp;

    /*Extend the empty heap with a free block of CHUNKSIZE bytes*/
	if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
		return -1;

	return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize; /*adjusted block size*/
    size_t extendize; /*amount to extend heap if no fit*/
    char *bp;

    if(size == 0) return NULL;

    if (size <= DSIZE) asize = 2 * DSIZE;
    else asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);

    if((bp = find_fit(asize)) != NULL){
        place(bp, asize);
        return bp;
    }

    /*if no fit found, get more memory and place the block*/
    extendize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendize/WSIZE)) == NULL){
        return NULL;
    }
    place(bp, asize);
    return bp;

}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));

    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    void *nextptr;
    size_t oldsize = GET_SIZE(HDRP(ptr)) - DSIZE;
    size_t newsize = ALIGN(size);
    size_t nextsize;

    if(oldptr == NULL) return mm_malloc(size);
    if(size == 0){
        mm_free(oldptr);
        return NULL;
    }
    if(newsize == oldsize) return oldptr;

    /*if size of to be allocated block(newsize) is smaller than old one(oldsize)*/
    if(newsize < oldsize){
        newptr = oldptr;
        if((oldsize - newsize) >= (2 * DSIZE)){ //split the block after allocation
            PUT(HDRP(newptr), PACK((newsize + DSIZE), 1));
            PUT(FTRP(newptr), PACK((newsize + DSIZE), 1));
            newptr = NEXT_BLKP(oldptr); 
            PUT(HDRP(newptr), PACK((oldsize - newsize), 0));
            PUT(FTRP(newptr), PACK((oldsize - newsize), 0));

            coalesce(newptr);
        }
        return oldptr;
    }
    
    /*if next block is free && size of current block + size of next block is larger than newsize*/
    nextptr = NEXT_BLKP(oldptr);
    if(nextptr != NULL && !GET_ALLOC(HDRP(nextptr))){
        nextsize = GET_SIZE(HDRP(nextptr)) - DSIZE;
        if(oldsize + nextsize > newsize){
            newptr = oldptr;

            if((oldsize + nextsize - newsize) >= (2 * DSIZE)){
                PUT(HDRP(newptr), PACK((newsize + DSIZE), 1));
                PUT(FTRP(newptr), PACK((newsize + DSIZE), 1));
                newptr = NEXT_BLKP(oldptr);
                PUT(HDRP(newptr), PACK((oldsize + nextsize + DSIZE - newsize), 0)); //(oldSize + DSIZE + nextSize + DSIZE) - (newSize + DSIZE)
                PUT(FTRP(newptr), PACK((oldsize + nextsize + DSIZE - newsize), 0));

                coalesce(newptr);
            }else{
                PUT(HDRP(newptr), PACK((oldsize + DSIZE + nextsize + DSIZE), 1));
                PUT(FTRP(newptr), PACK((oldsize + DSIZE + nextsize + DSIZE), 1));
            }

            return oldptr;
        }
    }

    /*neither next block is free nor newsize is smaller than oldsize*/
    newptr = mm_malloc(size);
    if(newptr == NULL) return NULL;
    memcpy(newptr, oldptr, oldsize);
    mm_free(oldptr);
    return newptr;
}

/******user defined function*******/
static void *extend_heap(size_t words){
    char *bp;
    size_t size;

    /*Allocate an even number of words to maintain alignment*/
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

    /*Initialize free block header/footer and the epilogue header*/
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

static void *coalesce(void *bp){
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); //get allocation status of the next block
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); //get allocation status of the previous block
    size_t cur_size = GET_SIZE(HDRP(bp)); // get size of the current block

    /*case 1*/
    if(prev_alloc && next_alloc){
        return bp;
    }
    /*case 2: free prev block, allocated next block*/
    if(!prev_alloc && next_alloc){
        cur_size += GET_SIZE(FTRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(cur_size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(cur_size, 0));
        bp = PREV_BLKP(bp);
    }
    /*case 3: allocated prev block, free next block*/
    else if(prev_alloc & !next_alloc){
        cur_size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(cur_size, 0));
        PUT(FTRP(bp), PACK(cur_size, 0));
    }
    /*case 4: both are free*/
    else{
        cur_size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(FTRP(PREV_BLKP(bp)));
        // PUT(FTRP(NEXT_BLKP(bp)), PACK(cur_size, 0));
        // PUT(HDRP(PREV_BLKP(bp)), PACK(cur_size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(cur_size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(cur_size, 0));
        bp = PREV_BLKP(bp);
    }

    if((next > (char *)bp) && (next < NEXT_BLKP(bp))) next = bp;

    return bp;

}

static void *find_fit(size_t asize){ /*use next fit strategy*/
    char *bp;

    /*search starts from next pointer(where last search ends)*/
    for(bp = next; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= asize)){
            next = bp;
            return bp;
        }
    }

    /*if cannot find fit block through next search, 
    then start search from very first block in an implicit list*/
    for(bp = heap_listp; bp < next; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= asize)){
            next = bp;
            return bp;
        }
    }

    return NULL; /*if no fit*/
}

static void place(void *bp, size_t asize){
    size_t size = GET_SIZE(HDRP(bp));

    if((size - asize) >= 2 * DSIZE){ /*split the block after allocation*/
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK((size - asize), 0));
        PUT(FTRP(bp), PACK((size - asize), 0));

        // next = bp;
    }else{ /*need not splitting*/
        PUT(HDRP(bp), PACK(size, 1));
        PUT(FTRP(bp), PACK(size, 1));
    }
}