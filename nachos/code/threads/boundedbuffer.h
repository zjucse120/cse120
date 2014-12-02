#include "synch.h" 
 
class BoundedBuffer  
{ 
         public: 
                BoundedBuffer(int maxsize); 
                ~BoundedBuffer(); 
                  
         
                void Read(char *data, int size); 

                void Write(char *data, int size); 
		
		void Close();
                 
         private: 
                 char *Buffer; 
                 int *element; 
                 int BufferSize; 
		 int number;
                 Lock *lock; 
                 Condition *BufferEmpty; 
                 Condition *BufferFull;    
}; 


