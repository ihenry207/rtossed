#ifndef USER_SYSCALLS_H
#define USER_SYSCALLS_H

#define UN_USED 1
#define TIME_SLEEP 2
#define IO_SLEEP 4
#define RUN 8
#define STOP 16
#define ZOMBIE 32

extern struct task_struct *current;

int microsleep(int microseconds);


#endif /* _SYSCALLS_H */
