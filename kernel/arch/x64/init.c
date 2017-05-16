#include <io/tty.h>
#include <printk.h>
#include <mm/frame.h>
#include <mm/memory_map.h>
#include <multiboot.h>
#include <mm/pages.h>
#include <mm/alloc.h>
#include <irq.h>
#include <task.h>
#include <io/pit.h>

void initialize(void *multiboot_information)
{
    tty_initialize();
    tty_set_color(TTY_COLOR_LIGHT_GREEN, TTY_COLOR_BLACK);
    initialize_irq();
    parse_multiboot(multiboot_information);
    initialize_frame_allocation();
    initialize_virtual_memory();
    initialize_kernel_heap();

    /*initialize_tasks();
    pit_initialize();
    printk("SYSTEM BOOT COMPLETE\n");

    int counter = 0;
    while (1) {
        for (int i = 0; i < 5000000; ++i) {
            __asm__ volatile ("nop");
        }
        printk("A %d\n", ++counter);
    }*/
}
