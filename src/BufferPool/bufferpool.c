/*!
 * \brief Managed pool of fixed sized buffers
 *
 * This is basically a fixed sized block allocator.
 * It can be used to manage a small number of fixed sized buffers dynamically
 * with the option to grow the number as needed with or without a maximum limit
 * on the number.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "bufferpool.h"

// Magic number to confirm this really is a buffer pool we are dealing with
#define BUFFERPOOLMAGIC 0x5533AADD

typedef struct tBufferPoolImpl tBufferPoolImpl;

// Header for an individual buffer
typedef struct tBufferPoolBufferItem
{
    uint32_t magic;                      //!< Magic number to identify a buffer pool item
    uint32_t unique;                     //!< Random number to identify a specific buffer pool
    struct tBufferPoolImpl* pBufferPool; //!< Buffer pool that owns this buffer item
    struct tBufferPoolBufferItem* pNext; //!< Next item or NULL
    // The actual buffer starts immediately after here in memory!
} tBufferPoolBufferItem;

// Internal representation of a buffer pool
struct tBufferPoolImpl
{
    uint32_t magic;                             //!< Magic number to identify a buffer pool item
    uint32_t unique;                            //!< Random number to identify a specific buffer pool
    const char* name;                           //!< Name of the pool
    struct tBufferPoolImpl* pNextPool;          //!< Head of the list of pools (used for debug & reporting only)
    tBufferPoolBufferItem* pBufferPoolFreeHead; //!< Head of the free list
    size_t bufferSize;                          //!< Size of the buffers in this pool
    uint32_t allocatedBuffers;                  //!< Total number of buffers allocated
    uint32_t freeBuffers;                       //!< Number of buffers currently free
    uint32_t maxBuffers;                        //!< Maximum allowed number of buffers (0 == unlimited)
    uint32_t outOfBuffers;                      //!< Count of how many times the max buffers limit has been hit
    uint32_t outOfMemory;                       //!< Count of how many out of memory errors there have been on this pool
    uint32_t totalAllocationRequests;           //!< Total number of requests for buffers
};

// Buffer pool list head. Used for debug and statistics
static tBufferPoolImpl* mpBufferPoolListHead = NULL;

/** Private functions **/

/*!
 * \brief Allocate a new buffer item
 */
static tBufferPoolBufferItem* bufferPoolAllocBufferItem(tBufferPoolImpl* pool)
{
    tBufferPoolBufferItem* bufferItem = NULL;
    if (pool->maxBuffers == 0 || pool->allocatedBuffers < pool->maxBuffers)
    {
        // Need to allocate more memory
        bufferItem = malloc(sizeof(tBufferPoolBufferItem) + pool->bufferSize);
        if (bufferItem)
        {
            bufferItem->magic = BUFFERPOOLMAGIC;
            bufferItem->unique = pool->unique;
            bufferItem->pBufferPool = pool;
            pool->allocatedBuffers++;
        }
        else
        {
            pool->outOfMemory++;
        }
    }
    else
    {
        // Reached the max block limit
        pool->outOfBuffers++;
    }

    return bufferItem;
}

/*!
 * \brief Add the buffer item to the free list
 */
static void bufferPoolAddToFreeList(tBufferPoolImpl* pool, tBufferPoolBufferItem* item)
{
    item->pNext = pool->pBufferPoolFreeHead;
    pool->pBufferPoolFreeHead = item;
    pool->freeBuffers++;
}

static tBufferPoolBufferItem* bufferPoolRemoveFromFreeList(tBufferPoolImpl* pool)
{
    tBufferPoolBufferItem* bufferItem = NULL;

    if (pool && pool->pBufferPoolFreeHead)
    {
        bufferItem = pool->pBufferPoolFreeHead;
        pool->pBufferPoolFreeHead = bufferItem->pNext;
        pool->freeBuffers--;
    }

    return bufferItem;
}

/** Public API **/

static void* bufferPoolAlloc(tBufferPool* bufferPool)
{
    void* buffer = NULL;
    tBufferPoolImpl* pool = bufferPool;

    if (pool && pool->magic == BUFFERPOOLMAGIC)
    {
        tBufferPoolBufferItem *bufferItem = bufferPoolRemoveFromFreeList(pool);

        pool->totalAllocationRequests++;

        if (bufferItem == NULL)
        {
            // Need to allocate more memory
            bufferItem = bufferPoolAllocBufferItem(pool);
        }

        if (bufferItem)
        {
            // The actual buffer is just after the buffer item
            buffer = (((uint8_t*)bufferItem) + sizeof(tBufferPoolBufferItem));
        }
    }

    return buffer;
}
static void* bufferPoolCalloc(tBufferPool* bufferPool)
{
    void *buffer = NULL;
    tBufferPoolImpl *pool = bufferPool;

    if (pool && pool->magic == BUFFERPOOLMAGIC)
    {
        buffer = bufferPoolAlloc(bufferPool);
        if (buffer)
        {
            memset(buffer, 0, pool->bufferSize);
        }
    }
    return buffer;
}

static void bufferPoolFree(void* buffer)
{
    bool success = false;

    if (buffer)
    {
        tBufferPoolBufferItem* bufferItem = (tBufferPoolBufferItem*)(((uint8_t*)buffer) - sizeof(tBufferPoolBufferItem));
        if (bufferItem->magic == BUFFERPOOLMAGIC)
        {
            tBufferPoolImpl* pool = bufferItem->pBufferPool;
            if (pool && pool->magic == BUFFERPOOLMAGIC && bufferItem->unique == pool->unique)
            {
                bufferPoolAddToFreeList(pool, bufferItem);
                success = true;
            }
        }

        if (!success)
        {
            printf("ERROR: Buffer pool failed to free. Leaking buffer!\n");
        }
    }
}

static bool bufferPoolPurgeFreeList(tBufferPool *bufferPool)
{
    bool freed = false;
    tBufferPoolImpl *pool = bufferPool;
    tBufferPoolBufferItem *bufferItem = bufferPoolRemoveFromFreeList(pool);

    while (bufferItem)
    {
        bufferItem->magic = 0;
        bufferItem->unique = 0;
        free(bufferItem);
        pool->allocatedBuffers--;
        freed = true;
        bufferItem = bufferPoolRemoveFromFreeList(pool);
    }

    return freed;
}

static tBufferPool* bufferPoolCreate(const char* name, const size_t bufferSize, const uint32_t preAllocation, const uint32_t maxAllocation)
{
    assert(bufferSize > 0);
    assert(maxAllocation == 0 || maxAllocation >= preAllocation);

    // The bufferSize must be greater than zero and if maxAllocation is not zero then it must be greater than maxAllocation
    if (bufferSize > 0 && (maxAllocation == 0 || maxAllocation >= preAllocation))
    {
        tBufferPoolImpl* bufferPool = calloc(sizeof(tBufferPoolImpl), 1);

        // Save the pool parameters
        bufferPool->maxBuffers = maxAllocation;
        bufferPool->bufferSize = bufferSize;

        // Set up the identity of the pool
        bufferPool->name = name;
        bufferPool->unique = rand(); // Random number to identify this pool
        bufferPool->magic = BUFFERPOOLMAGIC;

        // Add the new pool to the list of pools
        bufferPool->pNextPool = mpBufferPoolListHead;
        mpBufferPoolListHead = bufferPool;

        // Pre allocate any buffers requested
        for (uint32_t i = 0; i < preAllocation; i++)
        {
            tBufferPoolBufferItem* bufferItem = bufferPoolAllocBufferItem(bufferPool);
            if (bufferItem)
            {
                bufferPoolAddToFreeList(bufferPool, bufferItem);
            }
        }

        return (tBufferPool*)bufferPool;
    }

    return NULL;
}

static const char* bufferPoolGetName(tBufferPool *bufferPool)
{
    tBufferPoolImpl *pool = bufferPool;
    if (pool && pool->magic == BUFFERPOOLMAGIC)
    {
        return pool->name;
    }
    return NULL;
}

static void bufferPoolGetStats(tBufferPool *bufferPool, tBufferPoolStats *stats)
{
    tBufferPoolImpl *pool = bufferPool;
    if (pool && pool->magic == BUFFERPOOLMAGIC && stats != NULL)
    {
        stats->bufferSize = pool->bufferSize;
        stats->maxBuffers = pool->maxBuffers;
        stats->allocatedBuffers = pool->allocatedBuffers;
        stats->freeBuffers = pool->freeBuffers;
        stats->totalAllocationRequests = pool->totalAllocationRequests;
        stats->outOfBuffers = pool->outOfBuffers;
        stats->outOfMemory = pool->outOfMemory;
    }
}

static void bufferPoolPrintStats(tBufferPool* bufferPool)
{
    tBufferPoolImpl* pool = bufferPool;
    if (pool && pool->magic == BUFFERPOOLMAGIC)
    {
      printf("\n");
      printf("Buffer pool name            : %s\n", pool->name);
      printf("  Buffer size               : %zu bytes\n", pool->bufferSize);
      printf("  Max buffers               : %d (0 means unlimited)\n", pool->maxBuffers);
      printf("  Allocated buffers         : %d\n", pool->allocatedBuffers);
      printf("  Free buffers              : %d\n", pool->freeBuffers);
      printf("  Total allocation requests : %d\n", pool->totalAllocationRequests);
      printf("  Unable to allocate:\n");
      printf("    Max blocks reached      : %d\n", pool->outOfBuffers);
      printf("    Out of memory           : %d\n", pool->outOfMemory);
    }
}

static void bufferPoolDumpStats(void)
{
    int count = 0;
    printf("\nDumping stats for all buffer pools\n");
    for (tBufferPoolImpl* pool = mpBufferPoolListHead; pool != NULL; pool = pool->pNextPool)
    {
        bufferPoolPrintStats(pool);
        count++;
    }
    printf("\nTotal buffer pools: %d\n", count);
}

tBufferPoolController com_wadsweb_bufferpool =
{
    .create = &bufferPoolCreate,
    .alloc = &bufferPoolAlloc,
    .calloc = &bufferPoolCalloc,
    .free = &bufferPoolFree,
    .purgeFreeList = &bufferPoolPurgeFreeList,
    .getName = &bufferPoolGetName,
    .getStats = &bufferPoolGetStats,
    .printStats = &bufferPoolPrintStats,
    .dumpStats = bufferPoolDumpStats,
};
