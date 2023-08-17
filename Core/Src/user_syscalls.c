#include "user_syscalls.h"
#include "main.h"
#include "process.h"
#include "stm32h7xx_it.h" //for uwTick
//struct  task_struct init_struct;
//struct task_struct *current = &init_struct;

int microsleep(int microseconds)
{
	//first change process state to time sleep
	//change from microseconds to milliseconds
	uint32_t mills = (microseconds /1000);
	current->w_time = mills + uwTick; //setting up wake up time
	current->state |= TIME_SLEEP; //sleep the clock
	current->state &= ~RUN; //clear the running bit
	yield(); // yield the processor.
	return 1; //return nothing just so we don't get errors

}
