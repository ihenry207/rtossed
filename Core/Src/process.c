//define headers
#include "process.h"

#include "main.h"
#include "sh.h"
#include "progs.h"
#include <string.h>
#include "stm32h7xx_it.h"
#include "user_syscalls.h"

#define PROC_MAX 4
struct task_struct process_table[PROC_MAX]; //global varibale with size PROC_MAX only from functions within process.c
//int task_idle;
int kready = 0;

struct  task_struct init_struct;
struct task_struct *current = &init_struct; //pointing to the address of the task_struct
struct task_struct task_idle ;//keeps track of the context switches

void process_stack_init(struct task_struct* task)
{
	uint32_t* stack_ptr = (uint32_t*)task->r_struct.R13; //stack pointer 
	int i;
	//initialize the stack with 0s from FPSCR through S0

	for (i=0; i<16; i++)
	{
		*(--stack_ptr) = 0;
	}
	//push values onto the stack in reverse order
	*(--stack_ptr) = task->r_struct.xPSR;
	*(--stack_ptr) = task->r_struct.R15; //PC
	//*(--stack_ptr) = (uint32_t)task->cmd; //PC ortask->r_struct.R15
	//	*(--stack_ptr) = 0x00000000; //address of the link register ARM
	*(--stack_ptr) = task->r_struct.R14; //link register LR
	*(--stack_ptr) = task->r_struct.R12;
	*(--stack_ptr) = task->r_struct.R3;
	*(--stack_ptr) = task->r_struct.R2;
	*(--stack_ptr) = task->r_struct.R1;
	*(--stack_ptr) = task->r_struct.R0;

	//save the updated stack pointer variable back to the SP variable
	task->r_struct.R13 = (uint32_t) stack_ptr; //R13 is the stack pointer 

}
void process_table_init(void)
{

	memset(process_table, 0, sizeof(process_table)); //initialize the entire process to zero
	//set process_table[0] to execute shell
	struct task_struct* shell_task = (struct task_struct*) &process_table[0];
	shell_task->r_struct.R13 = (uint32_t)_eustack;
	shell_task->sp_start = (uint32_t)_eustack; //Do they mean R13
	shell_task->r_struct.R14 = 0;	//do they mean shell_task->r_struct.R14
	shell_task->r_struct.R15 = (uint32_t) process_start; //Do they mean R15
	shell_task->r_struct.xPSR = 0x01000000; //Do they mean xPSP
	shell_task->state = RUN; //in the declaration state is uint32_t
	shell_task->cmd = &process1;
	shell_task->exc_return = EXC_RETURN_THREAD_PSP; //in declaration it's int
	shell_task->pid = 0;	//pid_t type in declaration

	//initialize the stack for the shell task
	process_stack_init(shell_task);
	//since I don't know if I still need the shell_task i'll leave it on and create process1_struct
	struct task_struct* process1_task = (struct task_struct*) &process_table[1];
	process1_task->r_struct.R13 = (uint32_t)(_eustack-0x800);
	process1_task->sp_start = (uint32_t)(_eustack-0x800); //Do they mean R13
	process1_task->r_struct.R14 = 0;	//do they mean shell_task->r_struct.R14
	process1_task->r_struct.R15 = (uint32_t) process_start; //Do they mean R15
	process1_task->r_struct.xPSR = 0x01000000; //Do they mean xPSP
	process1_task->state = RUN; //in the declaration state is uint32_t
	process1_task->cmd = &process1;
	process1_task->exc_return = EXC_RETURN_THREAD_PSP; //in declaration it's int
	process1_task->pid = 1;	//pid_t type in declaration
	
	process_stack_init(process1_task);
	//initialize the idle task structure
	task_idle.state = STOP;
	task_idle.r_struct.xPSR = 0x01000000; //do they mean xpsr from r struct
	task_idle.exc_return = EXC_RETURN_THREAD_MSP_FPU;
	task_idle.pid = -2;
}
void process_start()
{
	//call function pointed to by cmd
	//	current->cmd(); //can be done different way if necessary
	int (*cmdLine)() = current->cmd;
	cmdLine();

	//if the function returns, change the process state to stop
	current->state = STOP;
	while (1){
		// looping indefinetly
	}


}

//schedular
struct task_struct *schedule_r()
{
	static int next_process_index = 0; //local variable to keep track of next process index
	//iterate through all the processes
	for (int i = 0; i < 4; i++)
	{	//round robin scheduling algo
		next_process_index = (next_process_index + 1) %4; 
		//check to see if process is in time_sleep
		if (process_table[next_process_index].state & TIME_SLEEP)
		{
			//check to see if enough time elapsed
			if (process_table[next_process_index].w_time <= uwTick)
			{
				//change process state from time sleep to runnable
				process_table[next_process_index].state &= ~TIME_SLEEP; 
				process_table[next_process_index].state |= RUN;
			}
		}
		if (process_table[next_process_index].state & RUN)
		{
			return &process_table[next_process_index];	

		}

	}
	//if no user is runnable return task_idle
	return &task_idle;
}


