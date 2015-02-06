#include "rtx.h"
#include "procs.h"

#define AVAILABLE_MEM_END 0x10008000

extern U32 Image$$RW_IRAM1$$ZI$$Limit;

typedef struct mem_blk_tag {
    struct mem_blk_tag *next;
} mem_blk;


mem_blk *availableMemory;


void k_rtx_init(void) {
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
    returnToCurrentContext();//run the first process
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
    int currPid=getCurrentPid();
    removeProcessFromQueue(currPid);
    addProcessToQueue(currPid);
    returnToCurrentContext();
    
    return 0;
}


int k_set_process_priority(int pid, int prio) {
    if (pid < 0 || pid >= MAX_PROCS || prio < 0 || prio > 3) {
        return -1;
    }
    removeProcessFromQueue(pid);
    setProcessPriority(pid,prio);
    addProcessToQueue(pid);
    
    return 0;
}

int k_get_process_priority(int pid) {
    if (pid < 0 || pid >= MAX_PROCS) {
        return -1;
    }
    return getProcessPriority(pid);
}
