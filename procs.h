#ifndef PROCS_H_
#define PROCS_H_ 1.0 //Version

#define MAX_PROCS 3

void addProcessToQueue(int pid);
void removeProcessFromQueue(int pid);
void setProcessPriority(int pid, int prio);
int getProcessPriority(int pid);
void returnToCurrentContext(void);
int getCurrentPid(void);
void initProcs(void(*procs[MAX_PROCS])(), int prios[MAX_PROCS]);

#endif
