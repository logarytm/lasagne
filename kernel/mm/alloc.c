#include <mm/alloc.h>
#include <mm/buddy.h>
#include <mm/pages.h>
#include <string.h>

static struct allocator heap_allocator;
const void *kernel_heap = 0xffffffff81000000;

void initialize_kernel_heap()
{
    size_t bitmap_size = allocator_bitmaps_size(KERNEL_HEAP_SIZE, KERNEL_HEAP_LEAF);
    map_range(kernel_heap, bitmap_size, MAP_RW | MAP_IMMEDIATE);
    map_range(kernel_heap + bitmap_size, KERNEL_HEAP_SIZE, MAP_RW | MAP_LAZY);
    allocator_init(&heap_allocator, kernel_heap + bitmap_size, KERNEL_HEAP_SIZE,
                   KERNEL_HEAP_LEAF, kernel_heap);
    allocator_print_status(&heap_allocator);
}
