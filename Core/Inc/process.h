#ifndef PROCESS_H
#define PROCESS_H
#include "stm32h7xx_hal.h"
#include <sys/types.h>

#define UN_USED 1
#define TIME_SLEEP 2
#define IO_SLEEP 4
#define RUN 8
#define STOP 16
#define ZOMBIE 32
extern int  kready;
extern const uint32_t _eustack[];
//extern process_table[]; //initialize the process table
extern struct task_struct *current ; //current global user space process
//extern int task_idle; //idle process
extern struct task_struct task_idle;
//void Process1(void);
//void Process2(void);
static inline void yield (void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	__DSB();
	__ISB();
}
struct r{
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R4;
	uint32_t R5;
	uint32_t R6;
	uint32_t R7;
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t R12;
	uint32_t R13;//stack pointer
	uint32_t R14; //Link Register
	uint32_t R15; // Program Counter (PC)
	uint32_t xPSR; //Program status Register



};
struct task_struct{
	uint32_t state;
	pid_t pid;
	int exc_return;
	uint32_t sp_start;
	int (*cmd)(void);
	struct r r_struct;
	uint32_t w_time; //wake up time new member
};
extern struct task_struct process_table[];
void process_stack_init(struct task_struct* task);
void process_table_init(void);
void process_start();
struct task_struct* schedule_r();

inline void Context_Register_Save(void)
{

	//push r4-r11 to the stack
	__asm volatile(
			"PUSH {r4-r11} \n\t"
		      );
	// the ':::' tells the compiler that the following registers may be modified by inline assembly code
}

//context restore registers
inline void Context_Restore_Registers(struct task_struct *point)
{
	__asm volatile("LDMIA %0, {r4-r11} \n\t" : : "r"(&(point->r_struct.R4))
		      );
}
//context switch return 
inline void Context_Switch_Return(struct task_struct *point)
{
	__asm volatile(
			"ldr pc, %0\n\t" : : "m" (point->exc_return)

		      );//load PC from exc

}



#endif /* PROCESS_H */
