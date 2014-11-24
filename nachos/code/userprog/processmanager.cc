#ifndef PROCESSMANAGER_H_
#define PROCESSMANAGER_H_
#include "processmanager.h"
#endif 
#include "copyright.h"
PCB::PCB(){
  void* object=(void*)(new int);
}

ProcessTable::ProcessTable(int size){
     bitmap = new BitMap(size);
     Pcb = new PCB[size];
     pm_lock = new Lock("pm lock");
}

int
ProcessTable::Alloc(void *object){
     int pid = bitmap->Find();
     Pcb[pid].object = object;
     return pid;
}

void* 
ProcessTable::Get(int index){
     void *pcb;
     pcb = Pcb[index].object;
     return pcb;
}

void 
ProcessTable::Release(int index){
      bitmap->Clear(index);
      Pcb[index].object=NULL;
}

/* PROCESSMANAGER_H_*/
