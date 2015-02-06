#include "procs.h"

typedef struct ProcessTAG {
    void (*proc)();
    int priority;
    int queueIndex;
    int pid;
} Process, *ProcessHANDLE;

static Process procsByPid[MAX_PROCS];
static ProcessHANDLE procQueue[MAX_PROCS];

void addProcessToQueue(int pid)
{
    ProcessHANDLE proc=&procsByPid[pid];
    
    int i,ii;
    i=MAX_PROCS - 1;
    while (i>0 && proc->priority <= procQueue[i]->priority)
        i--;
    //'i' is now the correct insertion index (TODO: check)
    for (ii=MAX_PROCS-1; ii>i; --ii)
        procQueue[ii]=procQueue[ii-1];
    procQueue[i]=proc;
    for (i=0; i<MAX_PROCS; ++i) {
        procQueue[i]->queueIndex = i;
    }
}

void removeProcessFromQueue(int pid)
{
    ProcessHANDLE proc=&procsByPid[pid];
    
    int i;
    for (i=proc->queueIndex; i<MAX_PROCS-1; ++i)
        procQueue[i]=procQueue[i+1];
    procQueue[MAX_PROCS-1]=proc;
    for (i=0; i<MAX_PROCS; ++i) {
        procQueue[i]->queueIndex = i;
    }
}

void setProcessPriority(int pid, int prio)
{
    procsByPid[pid].priority=prio;
}

int getProcessPriority(int pid)
{
    return procsByPid[pid].priority;
}

void returnToCurrentContext(void)
{
    procQueue[0]->proc();//TODO: return to PC
}

int getCurrentPid(void)
{
    return procQueue[0]->pid;
}

void initProcs(void(*procs[MAX_PROCS])(), int prios[MAX_PROCS])
{
    int i;
    for (i=0; i<MAX_PROCS; ++i)
    {
        procsByPid[i].proc=procs[i];
        procsByPid[i].priority=prios[i];
        procsByPid[i].pid=i;
        procsByPid[i].queueIndex=i;
        procQueue[i]=&procsByPid[i];
    }
    for (i=0; i<MAX_PROCS; ++i)
        addProcessToQueue(i);
}
