#include "rtx.h"
/********* TO BE PUT IN ITS OWN FILE *********/
#define NUM_PROCS 10
typedef struct ProcessTAG {
    void (*proc)(); //TODO: also save return PC
    int priority;
    int index;//index in priority queue
    int id;//index in pid queue
} Process, *ProcessHANDLE;

static Process procByPid[NUM_PROCS];
static Process procByPrio[NUM_PROCS];


void removeProcessFromQueue(int pid)
{
    Process proc=procByPid[pid];
    int i;
    for (i=proc.index; i<NUM_PROCS-1; ++i)
        procByPrio[i]=procByPrio[i+1];
    procByPrio[NUM_PROCS-1]=proc;//Technically pid is at the end, but it is out of place and will be knocked off on add
}

void addProcessToQueue(int pid)
{
    Process proc=procByPid[pid];

    int i,ii;
    for (i=0; i<NUM_PROCS; ++i)
    {
        if (procByPrio[i].priority>=proc.priority)
            break;
    }//'i' set by for loop above, used by for below
    //Now i is the correct index for insertion
    for (ii=NUM_PROCS-1; ii>i; --ii)
        if (ii>0)
            procByPrio[ii]=procByPrio[ii-1];
    procByPrio[i]=proc;
}

int getPriority(int pid)
{
    return procByPid[pid].priority;
}
Process* currentProc(void)
{
    return &procByPrio[0];
}
int currentPid(void)
{
    return procByPrio[0].id;
}
Process *getProc(int pid)
{
    return &procByPrio[pid];
}
//initProcs will be called from k_rtx_int
void initProcs(void (*procs[])(), int prios[])
{  
    int i;
    for (i=0; i<NUM_PROCS; ++i)
    {
        procByPrio[i].id=procByPid[i].id=i;
        procByPrio[i].index=procByPid[i].index=i;
        procByPrio[i].proc=procByPid[i].proc=procs[i];
        procByPrio[i].priority=procByPid[i].priority=prios[i];
    }
    for (i=0; i<NUM_PROCS; ++i)
        addProcessToQueue(i);
}
/*********************************************/
#define AVAILABLE_MEM_END 0x10008000

extern U32 Image$$RW_IRAM1$$ZI$$Limit;

typedef struct mem_blk_tag {
    struct mem_blk_tag *next;
} mem_blk;


mem_blk *availableMemory;


void k_rtx_init(void) {
    Process* currProc;
    U32 remainingMemoryStart;
    
    // Calculate number of available memory blocks
    U32 memoryStart = (unsigned int)&Image$$RW_IRAM1$$ZI$$Limit;
    U32 totalMemorySize = AVAILABLE_MEM_END - memoryStart;
    int numberOfRemainingMemoryBlocks = totalMemorySize / MEM_BLOCK_SIZE;
    
    // Initialize availableMemory
    remainingMemoryStart = memoryStart;
    availableMemory = (mem_blk*)remainingMemoryStart;
    while (numberOfRemainingMemoryBlocks--) {
        mem_blk *newBlock = (mem_blk*)remainingMemoryStart;
        newBlock->next = availableMemory;
        availableMemory = newBlock;
        remainingMemoryStart += MEM_BLOCK_SIZE;
    }
    currProc=currentProc();
    currProc->proc();
}

void *k_request_memory_block(void) {
    // atomic(on);
    mem_blk *returnBlock;
    
    if (!availableMemory) {
        /*
        while ( no memory block is available ) {
            put PCB on blocked_resource_q ;
            set process state to BLOCKED_ON_RESOURCE ;
            release_processor ( ) ;
        }
        */
        return 0;
    }
    returnBlock = availableMemory;
    availableMemory = availableMemory->next;
    
    // atomic(off);
    return returnBlock;
}

int k_release_memory_block(void *memory_block) {
    // atomic(on);
    
    if (!memory_block) {
        return -1;
    }
    ((mem_blk*)memory_block)->next = availableMemory;
    availableMemory = memory_block;
    
    /*
    if ( blocked on resource q not empty ) {
        handle_process_ready ( pop ( blocked resource q ) ) ;
    }
    */
    
    // atomic(off);
    return 0;
}

int k_release_processor(void) {
    Process *ret;
    int proc=currentPid();
    removeProcessFromQueue(proc);
    //fix ordering ?? 
    addProcessToQueue(proc);
    ret=currentProc();
    //ret=procByPrio[1];
    ret->proc();
    
    return 0;
}


int k_set_process_priority(int pid, int prio) {
    Process *p;
    if (pid < 0 || pid >= NUM_PROCS || prio < 0 || prio > 3) {
        return -1;
    }
    p=getProc(pid);
    removeProcessFromQueue(pid);


    p->priority=prio;
    addProcessToQueue(pid);
    
    return 0;
}

int k_get_process_priority(int pid) {
    if (pid < 0 || pid >= NUM_PROCS) {
        return -1;
    }
    return getPriority(pid);
}
