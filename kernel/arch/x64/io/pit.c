#include <io/pit.h>
#include <irq.h>
#include <io/ports.h>
#include <printk.h>
#include <task.h>

static void set_counter()
{
    outb(PIT_CH0, 0xFF);
    outb(PIT_CH0, 0xFF);
}

void pit_initialize()
{
    printk("hello pit world\n");
    set_irq_handler(0x20, pit_handler, 0);
    // set mode
    outb(PIT_COMMAND, PIT_CH0_SQUARE);

    set_counter();
    pic_flip_irq(0);
}

__attribute__((optimize("-fomit-frame-pointer"))) void pit_handler()
{
    irq_eoi();
    preempt_int();
}
