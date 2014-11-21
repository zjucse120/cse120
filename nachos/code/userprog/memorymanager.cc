#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_
#include "memorymanager.h"
#endif
#include "copyright.h"
#include "system.h"

MemoryManager::MemoryManager(int numPages)
{
    Num_Pages = numPages;
    bitmap = new BitMap(Num_Pages);
    MM_lock = new Lock("MemoryManager_Lock");

}

int 
MemoryManager::AllocPage(){
    MM_lock->Acquire();
    int i;
    i = bitmap->Find();
    if(i>0)
    bitmap->Mark(i);
    MM_lock->Release();
    return i;
}

void 
MemoryManager::FreePage(int physPageNum){
     MM_lock->Acquire();
     bitmap->Clear(physPageNum);
     MM_lock->Release();
}

bool 
MemoryManager::PageIsAllocated(int physPageNum){
    int i,p;
    i = physPageNum;
    MM_lock->Acquire();
    p = bitmap->Test(i);
    return p;
    MM_lock->Release();
}

int 
MemoryManager::NumPagesCanBeUsed(){
    int count;
    MM_lock->Acquire();
    count = bitmap->NumClear();
    MM_lock->Release();
    return count;
}
