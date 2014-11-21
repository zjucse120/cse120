// synch.cc
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    while (value == 0) { 			// semaphore not available
//        queue->Append((void *)currentThread);	// so go to sleep
    	queue->SortedInsert((void *)currentThread, -currentThread->getPriority()); 
        currentThread->Sleep();
    }
    value--; 					// semaphore available,
    // consume its value

    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) { 
    name = debugName;
    held=0; //Initial lock->0 
    holder=NULL;               
    queue = new List;
}
Lock::~Lock() {
   ASSERT(queue == NULL);
    delete queue;
    delete holder;
}
//priority
//
//

void Lock::Acquire() 
{
    ASSERT(!isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    //holder = NULL;
       while(held == 1){
    //	queue->Append((void *)currentThread);	// so go to sleep
	if(currentThread->getPriority() > holder->getPriority()){
		//if the priority is larger than lock holder's
		currentThread->dependThread = holder;
		holder->setPriority(currentThread->getPriority());
		} // donate priority 
    	queue->SortedInsert((void *)currentThread, -currentThread->getPriority());  //  sorted in wait queue
    	currentThread->Sleep();
    }	
	currentThread->dependThread = NULL ;
	held = 1;
        holder = currentThread;	
	initialPriority = holder->getPriority();
    (void) interrupt->SetLevel(oldLevel);

}
void Lock::Release() 
{
    ASSERT(isHeldByCurrentThread());
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
//    holder = currentThread;
    currentThread->setPriority(initialPriority);
    held = 0;
    holder = NULL;
   
    if(!(queue->IsEmpty())){
    	thread = (Thread *)queue->Remove();
    	scheduler->ReadyToRun(thread);
	}
    (void) interrupt->SetLevel(oldLevel);

}

//

bool Lock::isHeldByCurrentThread()
{
     if(holder == currentThread) 
        return true;
     return false;
}
Condition::Condition(char* debugName) {
     name = debugName;
     queue = new List; 
}
Condition::~Condition() { 
     ASSERT(queue==NULL);
     delete queue;
 
}
void Condition::Wait(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    conditionLock->Release();         //First,Release
//    queue->Append((void *)currentThread);// Sleep
    queue->SortedInsert((void *)currentThread, -currentThread->getPriority()); 
    currentThread->Sleep();
    conditionLock->Acquire();        // Then, Acquire
    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread *thread;
    thread = (Thread *)queue->Remove();
    if (thread != NULL)	
        scheduler->ReadyToRun(thread);
    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) { 
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread *thread;
    thread = (Thread *)queue->Remove();
    while(thread!=NULL){
    scheduler->ReadyToRun(thread);
    thread = (Thread *)queue->Remove();
    }
    (void)interrupt->SetLevel(oldLevel);
}


Mailbox::Mailbox(char* debugName){
    name = debugName;
    queue = new List;
    Mail_Lock = new Lock("MailLock");
    Send_Counter = 0;
    Receive_Counter = 0;
    Mail_Counter = 0;
    buffer = NULL;
    Mail_Condition = new Condition("MailCondition");
    Send_Condition = new Condition("SendCondition");
    Receive_Condition = new Condition("MailCondition");
    
}
Mailbox::~Mailbox(){
    delete Mail_Lock;
    delete Send_Condition;
    delete Receive_Condition;
    delete Mail_Condition;
    delete queue;
}
void Mailbox::Send(int message){
    Send_Counter++;                        // number of sender plus 1
    Mail_Lock->Acquire();
    while(Receive_Counter==0){
    Send_Condition->Wait(Mail_Lock);       // sender has to wait for a receiver
    }
    Receive_Counter--;                     // one receiver is matched by this sender
    Receive_Condition->Signal(Mail_Lock);  // wake up a receiver if there is one waiting for a sender
    Mail_Lock->Release();
    Mail_Lock->Acquire();
    while(Mail_Counter==1){                //Mail_Counter==1 means buffer can not be changed.
    Send_Condition->Wait(Mail_Lock);       // waiting for receiver done receiveing message from former sender
    }       
    buffer=&message;                       // message->buffer
    Mail_Counter = 1;
    Mail_Condition->Signal(Mail_Lock);     //wake up a receiver who is waiting for sender done sending message to buffer
    Mail_Lock->Release();
        
}

void Mailbox::Receive(int* message){
    Receive_Counter++;                    //number of receiver plus 1
    Mail_Lock->Acquire();
    while(Send_Counter==0){
    Receive_Condition->Wait(Mail_Lock);   //receiver has to wait for a sender
    }
    Send_Counter --;                      //one sender is matched by a this receiver
    Send_Condition->Signal(Mail_Lock);    //wake up a sender if there is one waiting for a receiver
    Mail_Lock->Release();
    Mail_Lock->Acquire();
    while(Mail_Counter==0){               //Mail_Counter==0 means buffer haven't get message from sender
    Mail_Condition->Wait(Mail_Lock);      //wait for sender done sending message
    }
    *message = *buffer;                   //message-> buffer outside
    Mail_Counter = 0;                              
    Send_Condition->Signal(Mail_Lock);    //wake up a sender who is waiting for receiver done receiving message to buffer outside
    Mail_Lock->Release();
}

//-------------------------------------------------------------------
//Problem 5 Mating Whales(Using Semaphore)
//-------------------------------------------------------------------

Whale::Whale(char* debugName){
    name = debugName;
    queue = new List;   
    Whale_lock1 = new Lock("Whalelock") ;
    Whale_lock2 = new Lock("Whalelock2");

    Malenum = 0;
    Femalenum = 0;
    Matchmakernum = 0;  //Initial value
    Male_Sema = new Semaphore("Male_Sema", 0);    
    Female_Sema = new Semaphore("Female_Sema", 0);
    Matchmaker_Sema = new Semaphore("Matchmaker_Sema", 0);
    Maleend_Sema = new Semaphore("Maleend_Sema", 0);
    Femaleend_Sema = new Semaphore("Femaleend_Sema", 0);
    Matchmakerend_Sema = new Semaphore("Matchmakerend_Sema", 0);
}

Whale::~Whale(){
    delete Male_Sema;
    delete Female_Sema;
    delete Matchmaker_Sema;
    delete Maleend_Sema;
    delete Femaleend_Sema;
    delete Matchmakerend_Sema;
    delete queue;
}

void Whale::Male()
{  
   Whale_lock1->Acquire(); 
    Malenum++;
   Whale_lock1->Release();
    while((Femalenum == 0 || Matchmakernum == 0) && !currentThread->waiting)    //There is no female or matchmaker, cannot mate.
    {
       currentThread->waiting = 1;   //Current whale is waiting for a mating. 
       Male_Sema->P();   
    } 
    if(currentThread->waiting)     //The waiting male whale mates successfully.
    {
       Whale_lock2->Acquire();
       Maleend_Sema->V();      
       Malename = currentThread->getName();    //Acquire the name of the mating male.
       Whale_lock2->Release();
    }
    else                      //The male whale mates with a waiting female successfully.
    {
       
       Whale_lock1->Acquire();
       Malenum--;
       Femalenum--;
       Matchmakernum--;      //A mating has been done, the numbers of whales that can be mated and matchmakers minus 1 respectively.
       Female_Sema->V();     //Put a female whale on the mating ready list.
       Matchmaker_Sema->V();    //Put a male whale on the mating ready list.
       Femaleend_Sema->P();     //A female whale has been mated.
       Matchmakerend_Sema->P();       //A matchmaker has participated in the mating.
       Whale_lock2->Acquire();
       Malename = currentThread->getName();    //Acquire the name of the mating male.
       printf("Thread %s and Thread %s mate successfully.\n", Malename, Femalename);
       Whale_lock2->Release();
       Whale_lock1->Release();

    }
}

void Whale::Female()
{
   
    Whale_lock1->Acquire();
     Femalenum++;
    Whale_lock1->Release();
    while((Malenum == 0 || Matchmakernum == 0) && !currentThread->waiting)  //There is no male or matchmaker, cannot mate.
    {
      currentThread->waiting = 1;    //Current whale is waiting for a mating. 
      Female_Sema->P();    
    }
    if(currentThread->waiting)       //The waiting female whale mates successfully.
    {
       Whale_lock2->Acquire();
       Femaleend_Sema->V();
       Femalename = currentThread->getName();     //Acquire the name of the mating female.
       Whale_lock2->Release();
    }
    else             //The female whale mates with a waiting male successfully.
    {
       
       Whale_lock1->Acquire();
       Malenum--;
      Femalenum--;
      Matchmakernum--;         //A mating has been done, the numbers of whales that can be mated and matchmakers minus 1 respectively.
       Male_Sema->V();        //Put a male whale on the mating ready list.
       Matchmaker_Sema->V();       //Put a matchmaker whale on the mating ready list.
       Maleend_Sema->P();          //A male whale has been mated.
       Matchmakerend_Sema->P();      //A matchmaker has participated in the mating.
       Whale_lock2->Acquire();
       Femalename = currentThread->getName();     //Acquire the name of the mating female.
       printf("Thread %s and Thread %s mate successfully.\n", Malename, Femalename);
       Whale_lock2->Release();
       Whale_lock1->Release();

    }


}

void Whale::Matchmaker()
{    
   Whale_lock1->Acquire();
    Matchmakernum++;
   Whale_lock1->Release();
    while((Malenum == 0 || Femalenum == 0) && !currentThread->waiting) //There is no male or female, cannot mate.
    {
       currentThread->waiting = 1;      //Current matchmaker is waiting for a male and a female. 
      Matchmaker_Sema->P();   
    }
    if(currentThread->waiting)        //The waiting matchmaker helps a male whale and a female whale mate successfully.
    {
     Whale_lock2->Acquire(); 
      Matchmakerend_Sema->V();
     Whale_lock2->Release();     
       
    }
    else                      //The matchmaker helps a waiting male whale and a waiting female whale mate successfully.
    {           
     Whale_lock1->Acquire(); 
      Malenum--;
     Femalenum--;
     Matchmakernum--;       //A mating has been done, the numbers of whales and matchmakers that can be mated minus 1 respectively.
       Male_Sema->V();        //Put a male whale on the mating ready list.
       Female_Sema->V();         //Put a female whale on the mating ready list.
       Maleend_Sema->P();         //A male whale has been mated.
       Femaleend_Sema->P();        //A female whale has been mated.
       Whale_lock2->Acquire();
       printf("Thread %s and Thread %s mate successfully.\n", Malename, Femalename);
       Whale_lock2->Release();
       Whale_lock1->Release();

    }
}

//---------------------------------------------------------
//Problem5 Mating Whales End
//---------------------------------------------------------
