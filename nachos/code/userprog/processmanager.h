#include "copyright.h"
#include "bitmap.h"
#include "synch.h"
class PCB{
public:  
  PCB();  
  void* object;
};

class ProcessTable{
public:
      ProcessTable(int size);
     
      int Alloc(void *object);
       
      void  *Get(int index);
      
      void Release(int index); 
      
      PCB *Pcb;
private:
      BitMap *bitmap;
      Lock *pm_lock;
};


