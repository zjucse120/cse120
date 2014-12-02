#include "boundedbuffer.h" 
 
BoundedBuffer::BoundedBuffer(int maxsize) 
{ 
     lock = new Lock("BufferLock"); 
     BufferEmpty = new Condition("BufferEmpty"); 
     BufferFull = new Condition("BufferFull"); 
     Buffer = new char[maxsize]; 
     BufferSize = maxsize; 
     number = 0; 
     element = new int[maxsize]; 
     for ( int i=0; i<=maxsize; i++ ) 
         element[i] = 0; 
} 
 
BoundedBuffer::~BoundedBuffer()   
{ 
      delete Buffer; 
      delete lock; 
      delete BufferEmpty; 
      delete BufferFull; 
      delete element; 
} 
 
void BoundedBuffer::Read(char *data, int size) 
{ 
     lock->Acquire(); 
     for ( int i=0,j=0; i<BufferSize && j<size; i++,j++ ) 
     { 
         if ( number == 0 ) 
         { 
              BufferEmpty->Wait(lock); 
              i = 0; 
         } 
         else if ( element[i] == 1 ) 
              { 
                   data[i] = Buffer[j]; 
                   element[i] = 0; 
                   j++; 
                   number--; 
                   if ( number == BufferSize-1 ) 
                        BufferFull->Signal(lock); 
              } 
     } 
     lock->Release(); 
} 
 
void BoundedBuffer::Write(char *data, int size) 
{ 
     lock->Acquire(); 
     for ( int i=0,j=0; i<BufferSize && j<size; i++,j++ ) 
     { 
         if ( number == BufferSize ) 
         { 
              BufferFull->Wait(lock); 
              i = 0; 
         } 
         else if ( element[i] == 0 ) 
              { 
                   Buffer[i] = data[j]; 
                   element[i] = 1; 
                   j++; 
                   number++; 
                   if ( number == 1 ) 
                        BufferEmpty->Signal(lock); 
              } 
     } 
     lock->Release(); 
} 

void
BoundedBuffer::Close(){
	
}               
