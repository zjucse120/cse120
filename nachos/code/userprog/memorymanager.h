#include "copyright.h"
#include "bitmap.h"
#include "synch.h"

class MemoryManager{
public:
     MemoryManager(int numPages);

     int AllocPage();

     void FreePage(int physPageNum);

     bool PageIsAllocated(int physPageNum);
     
     int NumPagesCanBeUsed();

private:
     BitMap* bitmap;
     int Num_Pages;
     Lock* MM_lock;
};
