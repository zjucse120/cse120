#include "copyright.h"
#include "bitmap.h"
#include "synch.h"

class ProcessTable{
pubilc:
      ProcessTable(int size);
     
      int Alloc(void *object);
       
      void  *Get(int index);
      
      void Release(index);
       
      void AdjustPCRegs();

private:
      BitMpa *bitmap;

}:


