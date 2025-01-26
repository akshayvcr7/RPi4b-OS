#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096

#define NR_TASKS				64

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

/* Used in tsk->state: */
#define TASK_RUNNING			0x0000
#define TASK_INTERRUPTIBLE		0x0001
#define TASK_UNINTERRUPTIBLE	0x0002

#define DEFINE(sym, val) \
	asm volatile("\n.ascii \"->" #sym " %0 " #val "\"" : : "i" (val))

extern struct task_struct *current;
extern struct task_struct * task[NR_TASKS];
extern int nr_tasks;

struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

struct thread_struct {
	struct cpu_context	cpu_context;	/* cpu context */
};


struct task_struct {
    /* CPU-specific state of this task: */
	struct cpu_context cpu_context;
	long state;
	long counter;
	long priority;
	long preempt_count;

	/*
	 * WARNING: on x86, 'thread_struct' contains a variable-sized
	 * structure.  It *MUST* be at the end of 'task_struct'.
	 *
	 * Do not put anything below here!
	 */
};

extern void sched_init(void);
extern void schedule(void);
void _schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void ret_from_fork(void);
void kernel_process(char *array);


#define INIT_TASK \
{/*cpu_context*/{0,0,0,0,0,0,0,0,0,0,0,0,0},/* state etc */0,0,1,0 }

#endif
#endif