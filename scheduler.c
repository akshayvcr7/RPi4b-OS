#include "include/scheduler.h"
#include "heap.h"
#include "reg.h"

int move_to_user_mode(uintptr_t pc);
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

int fork_process(unsigned long clone_flags, uintptr_t fn, uintptr_t arg, unsigned long stack)
{
	preempt_disable();
	struct task_struct *p;

	p = (struct task_struct *) kmalloc(4096U);
    kprintf("forked_process is 0x%x", p);
	if (!p)
		return -1;

	struct pt_regs *childregs = task_pt_regs(p);
	memset((unsigned long)childregs, 0,  sizeof(struct pt_regs));
	memset((unsigned long)&p->cpu_context, 0, sizeof(struct cpu_context));

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		struct pt_regs * cur_regs = task_pt_regs(current);
		//*childregs = *cur_regs;
		childregs->regs[0] = 0;
		childregs->sp = stack + HEAP_BLOCK_SIZE;
		p->stack = stack;
	}
	p->flags = clone_flags;

	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

    kprintf("fork fn 0x%x\n", fn);
	p->cpu_context.pc = (uintptr_t)&ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;
	int pid = nr_tasks++;
	task[pid] = p;
	preempt_enable();

	return 0;
}

void user_process() {
	//char buf[30] = {0};
	//ksprintf(buf, "User process started\n\r");
	call_sys_write("User process started\n\r");

	call_sys_exit();
}

void kernel_process(char *array)
{
	kprintf("Kernel process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((unsigned long)&user_process);
	if (err < 0){
		kprintf("Error while moving process to user mode\n\r");
	}
}

int move_to_user_mode(uintptr_t pc)
{
	struct pt_regs *regs = task_pt_regs(current);
	memset((unsigned long)regs, 0, sizeof(*regs));
	regs->pc = pc;
	regs->pstate = PSR_MODE_EL0t;
	unsigned long stack = kmalloc(4096); //allocate new user stack
	if (!stack) {
		return -1;
	}
	regs->sp = stack + HEAP_BLOCK_SIZE;
	current->stack = stack;
	return 0;
}

struct pt_regs * task_pt_regs(struct task_struct *tsk){
	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)p;
}

void exit_process(){
	preempt_disable();
	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}
	if (current->stack) {
		//free_page(current->stack);
	}
	preempt_enable();
	schedule();
}