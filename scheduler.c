#include "include/scheduler.h"
#include "heap.h"

static struct task_struct init_task = INIT_TASK;
extern struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;
//DEFINE(THREAD_CPU_CONTEXT, offsetof(struct task_struct, thread->cpu_context)); // offset of cpu_context in task_struct

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}

//TODO CFS scheduler
void _schedule(void)
{
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	switch_to(task[next]);
	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next)
{
	if (current == next)
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}


void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	enable_irq();
	_schedule();
	disable_irq();
}

int fork_process(uintptr_t fn, uintptr_t arg)
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) kmalloc(4096U);
    kprintf("forked_process is 0x%x", p);
	if (!p)
		return -1;
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

    kprintf("fork fn 0x%x\n", fn);
    p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (uintptr_t)&ret_from_fork;
	p->cpu_context.sp = (uintptr_t)p + HEAP_BLOCK_SIZE;
	int pid = nr_tasks++;
	task[pid] = p;
	preempt_enable();

	return 0;
}

void kernel_process(char *array)
{
	while (1){
        kprintf(array);
        kprintf("\n");
    for (int i = 0; i < 1000000; i++) {} //creates_delay
	}
}