#include <task.h>
#include <string.h>
#include <mm/memory_map.h>
#include <mm/alloc.h>
#include <printk.h>
#include <ds.h>

struct tss tss;


struct task kernel_task;
struct task *tasks;
struct thread *current_thread;

void do_sth()
{
    static int counter = 0;
    while (1) {
        for (int i = 0; i < 50000000; ++i) {
            __asm__ volatile ("nop");
        }
        printk("B %d\n", ++counter);
        __asm__ volatile ("int $48");
    }
}

void initialize_tasks()
{
    initialize_task(&kernel_task, "kernel", false, NULL);
    current_thread = kernel_task.main_thread;
    create_thread(&kernel_task, do_sth);
}

void initialize_task(struct task *task, char *name, bool userspace, void *main)
{
    task->name = name;
    task->userspace = userspace;
    task->main_thread = create_thread(task, main);
    CLIST_ADD(task->threads, task->main_thread);
    CLIST_ADD(tasks, task);
}

struct task *create_task(char *name, bool userspace, void *main)
{
    struct task *task = kalloc(sizeof *task);
    initialize_task(task, name, userspace, main);
    return task;
}

extern void switch_task_int_return();
struct thread *create_thread(struct task *task, void *main)
{
    struct thread *thread = kalloc(sizeof *thread);

    thread->rsp = (kalloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE);
    *(--thread->rsp) = 0x10; // ss
    uint64_t *place_for_rsp = --thread->rsp; // rsp
    *(--thread->rsp) = RFLAGS_IF; // rflags
    *(--thread->rsp) = 0x8; // cs
    *(--thread->rsp) = switch_task_int_return;
    *(--thread->rsp) = 0; // interrupt number
    for (int i = 0; i < 15; ++i) {
        *(--thread->rsp) = 0;
    }
    *(--thread->rsp) = main;
    *place_for_rsp = thread->rsp;
    /*uint64_t *entry = thread->rsp - 8;
    uint64_t *rflags = thread->rsp - 2 * 8;
    *entry = main;
    *rflags = RFLAGS_IF;
    thread->rsp -= 17 * 8; // all general registers w/o rsp + rflags + rip*/
    thread->state = THREAD_RUNNING;
    thread->task = task;

    CLIST_ADD(task->threads, thread);
    return thread;
}

void preempt_sys()
{
    struct thread *old_thread = current_thread;
    current_thread = current_thread->next;
    switch_task_sys(&old_thread->rsp, current_thread->rsp);
}

void setup_tss()
{

}

void set_current_kernel_stack(void *stack)
{
    //tss.esp0 = stack;
}

void preempt_int()
{
    struct thread *old_thread = current_thread;
    current_thread = current_thread->next;
    switch_task_int(&old_thread->rsp, current_thread->rsp);
}