#include <string.h>

#include "unity.h"
#include "bufferpool.h"

void test_CreateBufferPool(void)
{
    char* poolName = "test_create_buffer_pool";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 0, 0);

    tBufferPoolStats stats;
    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.totalAllocationRequests, "Total allocation requests incorrect\n");
}

void test_AllocateBuffer(void)
{
    char *poolName = "test_allocate_buffer";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 0, 0);

    void* buffer = com_wadsweb_bufferpool.alloc(bufferpool);

    tBufferPoolStats stats;
    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.totalAllocationRequests, "Total allocation requests incorrect\n");
}

void test_FreeBuffer(void)
{
    char *poolName = "test_free_buffer";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 0, 0);

    void *buffer = com_wadsweb_bufferpool.alloc(bufferpool);
    com_wadsweb_bufferpool.free(buffer);

    tBufferPoolStats stats;
    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.totalAllocationRequests, "Total allocation requests incorrect\n");
}

void test_CallocBuffer(void)
{
    char *poolName = "test_calloc_buffer";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 0, 0);

    void *buffer = com_wadsweb_bufferpool.alloc(bufferpool);
    memset(buffer, 0x55, 8);
    com_wadsweb_bufferpool.free(buffer);
    buffer = com_wadsweb_bufferpool.calloc(bufferpool);

    tBufferPoolStats stats;
    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(2, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    for (uint32_t i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, ((uint8_t*)buffer)[i], "Buffer not empty\n");
    }
}

void test_AllocMaxBuffer(void)
{
    char *poolName = "test_max_buffers";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 0, 3);

    void *buffer1 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer2 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer3 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer4 = com_wadsweb_bufferpool.alloc(bufferpool);

    tBufferPoolStats stats;
    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    TEST_ASSERT_NOT_NULL_MESSAGE(buffer1, "Buffer 1 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer2, "Buffer 2 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer3, "Buffer 3 is NULL\n");
    TEST_ASSERT_NULL_MESSAGE(buffer4, "Buffer 4 is not NULL\n");
}

void test_PreallocateBuffers(void)
{
    char *poolName = "test_preallocate_buffers";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 3, 0);

    void *buffer = com_wadsweb_bufferpool.alloc(bufferpool);

    tBufferPoolStats stats;
    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(2, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    TEST_ASSERT_NOT_NULL_MESSAGE(buffer, "Buffer is NULL\n");
}

void test_FixedSizePool(void)
{
    tBufferPoolStats stats;
    char *poolName = "test_fix_sized_pool";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 3, 3);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    void *buffer1 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer2 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer3 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer4 = com_wadsweb_bufferpool.alloc(bufferpool);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    TEST_ASSERT_NOT_NULL_MESSAGE(buffer1, "Buffer 1 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer2, "Buffer 2 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer3, "Buffer 3 is NULL\n");
    TEST_ASSERT_NULL_MESSAGE(buffer4, "Buffer 4 is not NULL\n");
}

void test_AllocTooManyFreeAndAllocAgain(void)
{
    tBufferPoolStats stats;
    char *poolName = "test_alloc_too_many_free_and_alloc_again";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 3, 3);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    void *buffer1 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer2 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer3 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer4 = com_wadsweb_bufferpool.alloc(bufferpool);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    TEST_ASSERT_NOT_NULL_MESSAGE(buffer1, "Buffer 1 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer2, "Buffer 2 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer3, "Buffer 3 is NULL\n");
    TEST_ASSERT_NULL_MESSAGE(buffer4, "Buffer 4 is not NULL\n");

    com_wadsweb_bufferpool.free(buffer1);
    buffer4 = com_wadsweb_bufferpool.alloc(bufferpool);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(3, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(1, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(5, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    TEST_ASSERT_NOT_NULL_MESSAGE(buffer4, "Buffer 4 is NULL\n");
}

void test_AllocFreeAndPurge(void)
{
    tBufferPoolStats stats;
    char *poolName = "test_alloc_free_and_purge";
    tBufferPool *bufferpool = com_wadsweb_bufferpool.create(poolName, 8, 0, 0);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_NOT_NULL_MESSAGE(bufferpool, "Buffer pool not created\n");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(poolName, com_wadsweb_bufferpool.getName(bufferpool), "Name incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(8, stats.bufferSize, "Buffer size incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.maxBuffers, "Max buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    void *buffer1 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer2 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer3 = com_wadsweb_bufferpool.alloc(bufferpool);
    void *buffer4 = com_wadsweb_bufferpool.alloc(bufferpool);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    TEST_ASSERT_NOT_NULL_MESSAGE(buffer1, "Buffer 1 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer2, "Buffer 2 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer3, "Buffer 3 is NULL\n");
    TEST_ASSERT_NOT_NULL_MESSAGE(buffer4, "Buffer 4 is NULL\n");

    com_wadsweb_bufferpool.free(buffer1);
    com_wadsweb_bufferpool.free(buffer2);
    com_wadsweb_bufferpool.free(buffer3);
    com_wadsweb_bufferpool.free(buffer4);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_EQUAL_MESSAGE(4, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.totalAllocationRequests, "Total allocation requests incorrect\n");

    com_wadsweb_bufferpool.purgeFreeList(bufferpool);

    com_wadsweb_bufferpool.getStats(bufferpool, &stats);

    TEST_ASSERT_EQUAL_MESSAGE(0, stats.freeBuffers, "Free buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.allocatedBuffers, "Allocated buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfBuffers, "Out of buffers incorrect\n");
    TEST_ASSERT_EQUAL_MESSAGE(0, stats.outOfMemory, "Out of memory incorrect memory\n");
    TEST_ASSERT_EQUAL_MESSAGE(4, stats.totalAllocationRequests, "Total allocation requests incorrect\n");
}

// Not really a test, just dumps all the stats to the console
void test_DumpPools(void)
{
    com_wadsweb_bufferpool.dumpStats();
}
