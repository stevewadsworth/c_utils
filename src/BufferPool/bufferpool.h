/*!
 * \brief Managed pool of fix sized buffers
 *
 * This is basically a fix sized block allocator.
 * It can be used to manage a small number of fix sized buffers dynamically
 * with the option to grow the number as needed with or without a maximum limit
 * on the number.
 *
 */
#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef void tBufferPool;

// Statistics about the buffer pool
typedef struct
{
    size_t bufferSize;                //!< Size of the buffers in this pool
    uint32_t allocatedBuffers;        //!< Total number of buffers allocated
    uint32_t freeBuffers;             //!< Number of buffers currently free
    uint32_t maxBuffers;              //!< Maximum allowed number of buffers (0 == unlimited)
    uint32_t outOfBuffers;            //!< Count of how many times the max buffers limit has been hit
    uint32_t outOfMemory;             //!< Count of how many out of memory errors there have been on this pool
    uint32_t totalAllocationRequests; //!< Total number of requests for buffers
} tBufferPoolStats;

typedef struct
{
    /*!
     * \brief Create a new buffer pool
     *
     * \param name The name to give the pool
     * \param bufferSize The size of the individual buffers in bytes
     * \param preAllocation How many buffers to initially create and add to the free list
     * \param maxAllocation The maximum number of buffers allowed in this pool
     * \returns New buffer pool or NULL
     */
    tBufferPool* (*create)(const char* name, const size_t bufferSize, const uint32_t preAllocation, const uint32_t maxAllocation);

    /*!
     * \brief Allocate a buffer
     *
     * Return a buffer from the free pool or allocate a new buffer if none are free
     * This is subject to the maximum allowed buffers.
     * The contents of the buffer may or may not be initialised.
     *
     * \param bufferPool The buffer pool to allocate from
     * \returns New buffer or NULL
     */
    void* (*alloc)(tBufferPool* bufferPool);

    /*!
     * \brief Allocate a buffer and zero the contents
     *
     * Return a buffer from the free pool or allocate a new buffer if none are free
     * This is subject to the maximum allowed buffers.
     * The buffer will be initialised with zeros.
     *
     * \param bufferPool The buffer pool to allocate from
     * \returns New buffer or NULL
     */
    void *(*calloc)(tBufferPool *bufferPool);

    /*!
     * \brief Release a buffer to the free pool
     *
     * \param buffer the buffer to release
     */
    void (*free)(void* buffer);

    /*!
     * \brief Return any buffers that are on the free list to the heap
     *
     * \param bufferPool The buffer pool to purge
     */
    bool (*purgeFreeList)(tBufferPool *bufferPool);

    /*!
     * \brief Get the name of the given buffer pool
     *
     * \param bufferPool The buffer pool to report on
     * \returns The name of the buffer pool
     */
    const char* (*getName)(tBufferPool *bufferPool);

    /*!
     * \brief Get the stats for the given buffer pool
     *
     * \param bufferPool The buffer pool to report on
     * \param stats A pointer to a tBufferPoolStats structure to be populated
     */
    void (*getStats)(tBufferPool *bufferPool, tBufferPoolStats* stats);

    /*!
     * \brief Print the stats for the given buffer pool
     *
     * \param bufferPool The buffer pool to report on
     */
    void (*printStats)(tBufferPool* bufferPool);

    /*!
     * \brief Print stats for all buffer pools
     */
    void (*dumpStats)(void);
} tBufferPoolController;

extern tBufferPoolController com_wadsweb_bufferpool;
