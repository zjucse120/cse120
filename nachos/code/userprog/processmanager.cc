#ifndef PROCESSMANAGER_H_
#define PROCESSMANAGER_H_

ProcessTable::ProcessTable(int size){
     bitmap = new BitMpa(size);
     //Locks = new Lock*[size];
     //Conditions = new Condition*[size];
}

int
ProcessTable::Alloc(void *object){
     int pid = bitmap->Find();
     PCB[pid] = object;
     return pid;
} 

void 
ProcessTable::*Get(int index){
     void *pcb;
     pcb = PCB[index];
     return pcb;
}

void 
ProcessTable::Release(int index){
      bitmap->Clear(index);
      PCB[index]=NULL;
}

void 
ProcessTable::AdjustPCRegs(){
     int pc = machine->ReadRegister(PCReg);
     machine->WriteRegister(PrevPCReg, pc);
     pc = machine->ReadRegister(NextPCReg);
     machine->WriteRegister(PCReg, pc);
     machine->WriteRegister(NextPCReg, pc + 4);
}

#endif /* PROCESSMANAGER_H_*/
