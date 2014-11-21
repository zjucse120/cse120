// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}
//----------------------------------------------------------------------
//LockTest1
//----------------------------------------------------------------------

Lock *locktest=NULL;
void
LockThread1(int param)
{
    printf("L1:0\n");
    locktest->Acquire();
    printf("L1:1\n");
    currentThread->Yield();
    printf("L1:2\n");
    locktest->Release();
    printf("L1:3\n");

}

void
LockThread2(int param)
{
    printf("L2:0\n");
    locktest->Acquire();
    printf("L2:1\n");
    currentThread->Yield();
    printf("L2:2\n");
    locktest->Release();
    printf("L2:3\n");

}

void
LockTest1()
{
    DEBUG('t', "Entering LockTest1");
   
    locktest = new Lock("LockTest1");
    Thread *t = new Thread("one");
    t->Fork(LockThread1, 0);
    t = new Thread("two");
    t->Fork(LockThread2, 0);
}
//----------------------------------------------------------------------
//Condition Variables Test 1
//
//----------------------------------------------------------------------

Lock *CVlocktest1 = NULL;
int conditionvariable = 0;
Condition *conditiontest1 = NULL;

void
CVThread1(int param){
    printf("L1:0 trying acquire lock\n");
    CVlocktest1->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable != 1)
    {
    conditiontest1->Wait(CVlocktest1);
    }
    printf("L1:2 get signal!\n");
    CVlocktest1->Release();
    printf("L1:3 Now you can run!\n");
    
}

void
CVThread2(int param){
   printf("L2:0 trying acquire lock\n");
   CVlocktest1->Acquire();
   printf("L2:1 done Acquire\n");
   conditionvariable = 1;
   printf("L2:2 trying to wake up L1\n");
   conditiontest1->Signal(CVlocktest1);
   printf("L2:3 done signal\n");
   CVlocktest1->Release();
   printf("L2:4 before printing this,L2 has done everything\n");
}

void
ConditionVariablesTest1(){
    DEBUG('t', "Entering ConditionVariablesTest1");
    CVlocktest1 = new Lock("ConditionLockTest1");
    conditiontest1 = new Condition("ConditionVariablesTest1");
    Thread *t = new Thread("one");
    t->Fork(CVThread1, 0);
    t = new Thread("two");
    t->Fork(CVThread2, 0);
}
//----------------------------------------------------------------------
//Lock Test 11
//----------------------------------------------------------------------

Lock *locktest1=NULL;
void
Locktest1Thread1(int param)
{
    printf("Thread1 is trying to acquire Locktest1.\n");
    locktest1->Acquire();
    printf("Thread1 is trying to acquire Lock Locktest1 for the second time.\n");
    locktest1->Acquire();

}



void
LockTest11()
{
    DEBUG('t', "Entering LockTest1");
   
    locktest1 = new Lock("LockTest1");
    Thread *t = new Thread("1");
    t->Fork(Locktest1Thread1, 0);
}

//----------------------------------------------------------------------
//Lock Test 12
//
//----------------------------------------------------------------------

Lock *locktest2=NULL;
void
Locktest2Thread1(int param)
{
    printf("Thread1 is trying to release Locktest2.\n");
    locktest2->Release();
}

void
LockTest12()
{
    DEBUG('t', "Entering LockTest2");
   
    locktest2 = new Lock("LockTest2");
    Thread *t = new Thread("1");
    t->Fork(Locktest2Thread1, 0);
}


//----------------------------------------------------------------------
//Lock Test 13
//
//----------------------------------------------------------------------

Lock *locktest3=NULL;
void
Locktest3Thread1(int param)
{
    printf("Thread1 is trying to acquire LockTest3.\n");
    locktest3->Acquire();
    printf("Delete LockTest3.\n");
    locktest3->~Lock();
    
}

void
LockTest13()
{
    DEBUG('t', "Entering LockTest3");
   
    locktest3 = new Lock("LockTest3");
    Thread *t = new Thread("1");
    t->Fork(Locktest3Thread1, 0);
}


//----------------------------------------------------------------------
//Condition Variables Test 14
// (4)waiting on a condition variable without holding a Lock
//----------------------------------------------------------------------

Lock *CVlocktest14 = NULL;
int conditionvariable14 = 0;
Condition *conditiontest14 = NULL;

void
CVThread114(int param){
    printf("L1:0 trying acquire lock\n");
    //CVlocktest1->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable != 1)
    {
    conditiontest1->Wait(CVlocktest1);
    }
    printf("L1:2 get signal!\n");
    CVlocktest1->Release();
    printf("L1:3 Now you can run!\n");
    
}

void
CVThread214(int param){
   printf("L2:0 trying acquire lock\n");
   CVlocktest1->Acquire();
   printf("L2:1 done Acquire\n");
   conditionvariable = 1;
   printf("L2:2 trying to wake up L1\n");
   conditiontest1->Signal(CVlocktest1);
   printf("L2:3 done signal\n");
   CVlocktest1->Release();
   printf("L2:4 before printing this,L2 has done everything\n");
}

void
ConditionVariablesTest14(){
    DEBUG('t', "Entering ConditionVariablesTest1");
    CVlocktest1 = new Lock("ConditionLockTest1");
    conditiontest1 = new Condition("ConditionVariablesTest1");
    Thread *t = new Thread("one");
    t->Fork(CVThread114, 0);
    t = new Thread("two");
    t->Fork(CVThread214, 0);
}
//----------------------------------------------------------------------
//Condition Variables Test 15
// (4)waiting on a condition variable without holding a Lock
//----------------------------------------------------------------------

Lock *CVlocktest15 = NULL;
int conditionvariable15 = 0;
Condition *conditiontest15 = NULL;

void
CVThread115(int param){
    printf("L1:0 trying acquire lock\n");
    CVlocktest15->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable15 != 1)
    {
    conditiontest15->Wait(CVlocktest15);
    }
    printf("L1:2 get signal!\n");
    CVlocktest15->Release();
    printf("L1:3 Now you can run!\n");
    
}
void
CVThread315(int param){
    printf("L11:0 trying acquire lock\n");
    CVlocktest15->Acquire();
    printf("L11:1 done Acquire\n");
    while(conditionvariable15 != 1)
    {
    conditiontest15->Wait(CVlocktest15);
    }
    printf("L11:2 get signal!\n");
    CVlocktest15->Release();
    printf("L11:3 Now you can run!\n");
    
}

void
CVThread215(int param){
   printf("L2:0 trying acquire lock\n");
   CVlocktest15->Acquire();
   printf("L2:1 done Acquire\n");
   conditionvariable15 = 1;
   printf("L2:2 trying to wake up L1\n");
   conditiontest15->Broadcast(CVlocktest15);
   printf("L2:3 done signal\n");
   CVlocktest15->Release();
   printf("L2:4 before printing this,L2 has done everything\n");
}

void
ConditionVariablesTest15(){
    DEBUG('t', "Entering ConditionVariablesTest1");
    CVlocktest15 = new Lock("ConditionLockTest1");
    conditiontest15 = new Condition("ConditionVariablesTest1");
    Thread *t = new Thread("one");
    t->Fork(CVThread115, 0);
    t = new Thread("two");
    t->Fork(CVThread315, 0);
    t = new Thread("three");
    t->Fork(CVThread215, 0);

}

//----------------------------------------------------------------------
//Condition Variables Test 16
//(6) signaling and broadcasting to a condition variable with no waiters is a no-op, and future threads that wait will block (i.e., the signal/broadcast is "lost")
//----------------------------------------------------------------------

Lock *CVlocktest16 = NULL;
int conditionvariable16 = 0;
Condition *conditiontest16 = NULL;

void
CVThread116(int param){
    printf("L1:0 trying acquire lock\n");
    currentThread->Yield();
    CVlocktest16->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable16 != 1)
    {
    conditiontest16->Wait(CVlocktest16);
    conditionvariable16 = 1;
    }
    printf("L1:2 get signal!\n");
    CVlocktest16->Release();
    printf("L1:3 Now you can run!\n");
    
}

void
CVThread216(int param){
   printf("L2:0 trying acquire lock\n");
   CVlocktest16->Acquire();
   printf("L2:1 done Acquire\n");
   //conditionvariable16 = 1;
   printf("L2:2 trying to wake up L1\n");
   conditiontest16->Signal(CVlocktest16);
   printf("L2:3 done signal\n");
   CVlocktest16->Release();
   printf("L2:4 before printing this,L2 has done everything\n");
}

void
ConditionVariablesTest16(){
    DEBUG('t', "Entering ConditionVariablesTest1");
    CVlocktest16 = new Lock("ConditionLockTest1");
    conditiontest16 = new Condition("ConditionVariablesTest1");
    Thread *t = new Thread("one");
    t->Fork(CVThread116, 0);
    t = new Thread("two");
    t->Fork(CVThread216, 0);
}
//----------------------------------------------------------------------
//Condition Variables Test 17
//(7)a thread calling Signal holds the Lock passed in to Signal
//----------------------------------------------------------------------
Lock *CVlocktest171 = NULL;
Lock *CVlocktest17 = NULL;
int conditionvariable17 = 0;
Condition *conditiontest17 = NULL;
int conditionvariable171 = 0;
Condition *conditiontest171 = NULL;

char *hold_lockname;
void
CVThread117(int param){
    printf("L1:0 trying acquire lock\n");
    CVlocktest17->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable17 != 1)
    {
    conditiontest17->Wait(CVlocktest17);
    }
    printf("L1:2 get signal!\n");
    CVlocktest17->Release();
    printf("L1:3 Now you can run!\n");
    
}
void
CVThread317(int param){
    printf("L1:0 trying acquire lock\n");
    CVlocktest171->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable171 != 1)
    {
    conditiontest171->Wait(CVlocktest171);
    }
    printf("L1:2 get signal!\n");
    CVlocktest171->Release();
    printf("L1:3 Now you can run!\n");
    
}

void
CVThread217(int param){
   printf("L2:0 trying acquire lock\n");
   CVlocktest17->Acquire();
   hold_lockname=CVlocktest17->getName();
   printf("Acquire the lock : %s \n",hold_lockname);
   printf("L2:1 done Acquire\n");
   conditionvariable17 = 1;
   printf("L2:2 trying to wake up L1\n");
   hold_lockname=CVlocktest171->getName();
   printf("The lock used to Signal : %s \n",hold_lockname);
   conditiontest17->Signal(CVlocktest171);
   printf("L2:3 done signal\n");
   CVlocktest17->Release();
   printf("L2:4 before printing this,L2 has done everything\n");
}

void
ConditionVariablesTest17(){
    DEBUG('t', "Entering ConditionVariablesTest1");
    CVlocktest17 = new Lock("ConditionLockTest17, different from the lock used in Signal");
    CVlocktest171 = new Lock("ConditionLockTest171, lock used to Siganl but failed");
    conditiontest17 = new Condition("ConditionVariablesTest1");
    conditiontest171 = new Condition("ConditionVariablesTest1");
    Thread *t = new Thread("one");
    t->Fork(CVThread117, 0);
    t = new Thread("two");
    t->Fork(CVThread317, 0);
    t = new Thread("three");
    t->Fork(CVThread217, 0);
}

//----------------------------------------------------------------------
//Condition Variables Test 18
// (8) deleting a lock or condition variable should have no threads on the wait queue
//
//----------------------------------------------------------------------

Lock *CVlocktest18 = NULL;
int conditionvariable18 = 0;
Condition *conditiontest18 = NULL;

void
CVThread118(int param){
    printf("L1:0 trying acquire lock\n");
    CVlocktest18->Acquire();
    printf("L1:1 done Acquire\n");
    while(conditionvariable18 != 1)
    {
    conditiontest18->Wait(CVlocktest18);
    }
    printf("L1:2 get signal!\n");
    CVlocktest18->Release();
    printf("L1:3 Now you can run!\n");
    
}

void
CVThread318(int param){

   printf("L3:0 Deleting lock\n");
   CVlocktest18->~Lock();
   printf("L3:1 Deleting Condition\n");
   conditiontest18->~Condition();
   printf("L3:2 Done deleting\n");
}

void
ConditionVariablesTest18(){
    DEBUG('t', "Entering ConditionVariablesTest1");
    CVlocktest18 = new Lock("ConditionLockTest1");
    conditiontest18 = new Condition("ConditionVariablesTest1");
    Thread *t = new Thread("one");
    t->Fork(CVThread118, 0);
    t = new Thread("three");
    t->Fork(CVThread318, 0);
}
//---------------------------------------------------------------------
//
//LBS basic function test of Mailbox.
//
//MailboxTest1
//
//---------------------------------------------------------------------

Mailbox* Mailbox1;

void
MailboxThread1(int param){
    printf("S1:0 Trying to send message\n");
    Mailbox1->Send(param);
    printf("S1:1 Done sending\n");
   
}

void
MailboxThread2(int param){
    int r;
    printf("R1:0 Tring to receieve message\n");
    Mailbox1->Receive(&r);
    printf("R2:1 Done receiving\n");
    printf("Typing what receives\n");
    printf("%d\n",r);
}
void 
MailboxTest1(){
    DEBUG('t', "Entering MailboxTest1");
    Mailbox1 = new Mailbox("MailboxTest1");
    Thread *t = new Thread("one");
    t->Fork(MailboxThread1, 1);
    t= new Thread("two");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Three");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Four");
    t->Fork(MailboxThread1, 2);
    t= new Thread("Five");
    t->Fork(MailboxThread1, 3);
    t= new Thread("Six");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Seven");
    t->Fork(MailboxThread1, 4);
    t= new Thread("Eight");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Nine");
    t->Fork(MailboxThread1, 5);
    t= new Thread("Ten");
    t->Fork(MailboxThread2, 0);
}
void 
MailboxTest2(){
    DEBUG('t', "Entering MailboxTest1");
    Mailbox1 = new Mailbox("MailboxTest1");
    Thread *t = new Thread("one");
    t->Fork(MailboxThread1, 1);
    t= new Thread("two");
    t->Fork(MailboxThread1, 2);
    t= new Thread("Three");
    t->Fork(MailboxThread1, 3);
    t= new Thread("Four");
    //t->Fork(MailboxThread1, 4);
    //t= new Thread("Five");
    //t->Fork(MailboxThread1, 5);
    //t= new Thread("Six");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Seven");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Eight");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Nine");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Ten");
    t->Fork(MailboxThread2, 0);
}
void 
MailboxTest3(){
    DEBUG('t', "Entering MailboxTest1");
    Mailbox1 = new Mailbox("MailboxTest1");
    Thread *t = new Thread("one");
    //t->Fork(MailboxThread2, 0);
    //t= new Thread("two");
    //t->Fork(MailboxThread2, 0);
    //t= new Thread("Three");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Four");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Five");
    t->Fork(MailboxThread2, 0);
    t= new Thread("Six");
    t->Fork(MailboxThread1, 1);
    t= new Thread("Seven");
    t->Fork(MailboxThread1, 2);
    t= new Thread("Eight");
    t->Fork(MailboxThread1, 3);
    t= new Thread("Nine");
    t->Fork(MailboxThread1, 4);
    t= new Thread("Ten");
    t->Fork(MailboxThread1, 5);
}



// The following tests are for the Join() function in part 3.
//Jointest1:child will wait when not being called even if it finishes
// The following tests are for the Join() function in part 3.
//Jointest1:child will wait when not being called even if it finishes
 void
Joiner1(Thread *joinee1)
{
    int num;
    printf("Parent thread is going to run as T1\n");

    for (num = 0; num < 7; num++) {
        printf("T1 is now working at line %d\n", num);
        currentThread->Yield();
    }
    currentThread->Yield();
    currentThread->Yield();
    currentThread->Yield();
    currentThread->Yield();
    
    joinee1->Join();
    printf("Joinee has finish executing, we can continue\n");
}

void
Joinee1()
{
    int num;
    printf("Child thread is going to run as T2\n");

    for (num = 0; num < 2; num++) {
        printf("T2 is now working at line %d\n", num);
        currentThread->Yield();
    }
    printf("Child Thread has finished work,wait for Join\n");
}

void
JoinThread1() {
  Thread *joiner1 = new Thread("joiner1", 0);  // will not be joined
  Thread *joinee1 = new Thread("joinee1", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner1->Fork((VoidFunctionPtr) Joiner1, (int) joinee1);
  joinee1->Fork((VoidFunctionPtr) Joinee1, 0);
  //       // this thread is done and can go on its merry way
   printf("Forked off the joiner and joiner threads.\n");
}


//Jointest2:if a parent calls Join on a child and the child is still executing, the parents waits
void
Joiner2(Thread *joinee2)
{
  currentThread->Yield();
  currentThread->Yield();

  printf("Waiting for the Joinee to finish executing.\n");
  joinee2->Join();

  printf("Joinee has finished executing, we can continue.\n");
}

void
Joinee2()
{
  int i;

  for (i = 0; i < 5; i++) {
    printf("Smell the roses.\n");
    currentThread->Yield();
  }

  currentThread->Yield();
  printf("Done smelling the roses!\n");
  currentThread->Yield();
}

void
JoinThread2()
{
  Thread *joiner2 = new Thread("joiner2", 0);  // will not be joined
  Thread *joinee2 = new Thread("joinee2", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner2->Fork((VoidFunctionPtr) Joiner2, (int) joinee2);
  joinee2->Fork((VoidFunctionPtr) Joinee2, 0);
  // this thread is done and can go on its merry way
  printf("Forked off the joiner and joiner threads.\n");
}

//Jointest3:if a parent calls Join on a child and the child has finished executing, the parent does not stop

void
Joiner3(Thread *joinee3)
{
    int num;
    printf("Parent thread is going to run as T1\n");
    
    currentThread->Yield();
    currentThread->Yield();
    currentThread->Yield();
    currentThread->Yield();

    joinee3->Join();
    printf("Joinee has finished executing, we can continue\n");
    for(num = 0; num < 2; num++)
        printf("T1 is now working at line %d\n",num);
}

void
Joinee3()
{
    int num;
    printf("Child thread is going to run as T2\n");

    for (num = 0; num < 2; num++) {
        printf("T2 is now working at line %d\n", num);
        currentThread->Yield();
    }
    printf("Child Thread has finished work,wait for Join\n");
}

void
JoinThread3() {
  Thread *joiner3 = new Thread("joiner3", 0);  // will not be joined
  Thread *joinee3 = new Thread("joinee3", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner3->Fork((VoidFunctionPtr) Joiner3, (int) joinee3);
  joinee3->Fork((VoidFunctionPtr) Joinee3, 0);
  //       // this thread is done and can go on its merry way
   printf("Forked off the joiner and joiner threads.\n");
}


//Jointest4:can not Join() yourself
void
JoinThread4() {
  Thread *joiner4 = new Thread("joiner4", 0);  // will not be joined
  Thread *joinee4 = new Thread("joinee4", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner4->Fork((VoidFunctionPtr) Joiner3, (int) joiner4);
  joinee4->Fork((VoidFunctionPtr) Joiner3, 0);
  //       // this thread is done and can go on its merry way
   printf("Forked off the joiner and joiner threads.\n");
}

//Jointest5: join is only invokeed on threads created to be joined.
void
JoinThread5() {
  Thread *joiner5 = new Thread("joiner5", 0);  // will not be joined
  Thread *joinee5 = new Thread("joinee5", 0);  // Will not be joined

  // fork off the two threads and let them do their business
  joiner5->Fork((VoidFunctionPtr) Joiner3, (int) joinee5);
  joinee5->Fork((VoidFunctionPtr) Joinee3, 0);
  //       // this thread is done and can go on its merry way
   printf("Forked off the joiner and joiner threads.\n");
}

//Jointest6: Join is only called on a thread that forked

void
JoinThread6() {
  Thread *joiner6 = new Thread("joiner6", 0);  // will not be joined
  Thread *joinee6 = new Thread("joinee6", 0);  // Will not be joined

  // fork off the two threads and let them do their business
  joiner6->Fork((VoidFunctionPtr) Joiner3, (int) joinee6);
 // this thread is done and can go on its merry way
}


//Jointest7: can only join once

void
Joiner7(Thread *joinee7)
{
    printf("Parent thread is going to run as T1\n");
    currentThread->Yield();

    printf("T1 is now working\n");
    currentThread->Yield();
  
    printf("For the first Join call\n");
    joinee7->Join();
    
    currentThread->Yield();
    
    printf("For the second Join call\n");
    joinee7->Join();
    printf("Joinee has finish executing, we can continue\n");
}

void
Joinee7()
{
    int num;
    printf("Child thread is going to run as T2\n");

    for (num = 0; num < 3; num++) {
        printf("T2 is now working at line %d\n", num);
        currentThread->Yield();
    }
}

void
JoinThread7() {
  Thread *joiner7 = new Thread("joiner7", 0);  // will not be joined
  Thread *joinee7 = new Thread("joinee7", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner7->Fork((VoidFunctionPtr) Joiner7, (int) joinee7);
  joinee7->Fork((VoidFunctionPtr) Joinee7, 0);
 // this thread is done and can go on its merry way
   printf("Forked off the joiner and joiner threads.\n");
}

//----------------------------------------------------------------------
//PriorityTest1
//----------------------------------------------------------------------

//Scheduler *scheduler1 = NULL;

void
PriorityThread1(int param)
{
	printf("L1");
}

void
PriorityThread2(int param)
{
	printf("L2");
}

void
PriorityThread3(int param)
{
	printf("L3");
     currentThread->Yield();
	printf("L3");
     currentThread->Yield();
	printf("L3");
}

void
PriorityTest1()
{
	DEBUG('t', "Entering PriorityTest1");

	Thread *t = new Thread("one");
	Thread *x = new Thread("two");
	Thread *y = new Thread("Three");
	t->setPriority(1);
	x->setPriority(10);
	y->setPriority(20);

	t->Fork(PriorityThread1, 0);
	
	x->Fork(PriorityThread2, 0);
	
	y->Fork(PriorityThread3, 0);
	//t = new Thread("four");
	//t->Fork(PriorityThread, -6);

}
//----------------------------------------------------------------------
//LBS Priority Test 2
//
//----------------------------------------------------------------------
Lock *plocktest2 = NULL;

void
PriorityThread4(int param)
{
     plocktest2->Acquire();
     printf("L4:1\n");
     //currentThread->Yield();
     printf("L4:2\n");
     plocktest2->Release();
     printf("L4:3\n");	
}

void
PriorityThread5(int param)
{
     plocktest2->Acquire();
     printf("L5:1 get lock\n");
//     currentThread->Yield();

     plocktest2->Release();
     printf("L5:3  release lock\n");	
}

void
PriorityThread6(int param)
{
     plocktest2->Acquire();
     printf("L6:1 get lock\n");

     plocktest2->Release();
     printf("L6:3  release lock\n");	
}

void
PriorityThread7(int param)
{    
Thread *y=new Thread("Three"); 
Thread *x=new Thread("Two");
     plocktest2->Acquire();
     printf("L7:1 get lock\n");
        y->setPriority(10);
	y->Fork(PriorityThread5, 0);
	printf("the priority of y is: %d\n", y->getPriority());
        currentThread->Yield();
	x->setPriority(15);
      	x->Fork(PriorityThread6, 0);
	printf("the priority of x is: %d\n", x->getPriority());
	currentThread->Yield();

     printf("L7:2  back from yield\n");
     plocktest2->Release();
     printf("L7:3  release lock\n");	
	
}


void
PriorityTest2()
{
	DEBUG('t', "Entering PriorityTest2");

	plocktest2 = new Lock("LockTest2");

	Thread *t = new Thread("one");
	Thread *z = new Thread("Four");

        t->setPriority(-6);
	printf("the priority of t is: %d\n", t->getPriority());
	t->Fork(PriorityThread4, 0);

	z->setPriority(6);
	z->Fork(PriorityThread7, 0);
	printf("the priority of z is: %d\n", z->getPriority());

}



//----------------------------------------------------------------------
// Priority Test 3
//----------------------------------------------------------------------

Lock *plocktest3 = NULL;
Lock *plocktest4 = NULL;
Condition *cond = NULL; 

void
PriorityThread8(int param)
{
     printf("This is PriorityThread8\n");
     plocktest3->Acquire();
     printf("L8:1\n");
     plocktest4->Acquire();
     cond->Broadcast(plocktest4);
     plocktest4->Release();
     currentThread->Yield();
    
     printf("L8:2\n");	

     plocktest3->Release();
     printf("L8:3\n");	
}

void
PriorityThread9(int param)
{
   // currentThread->Yield();
 int num;
     printf("This is PriorityThread9\n");
     plocktest4->Acquire();
     cond->Wait(plocktest4);
     plocktest4->Release();
    //currentThread->Yield();
  for (num = 0; num < 10; num++) {
    printf("PriorityThread9 looped %d times\n",num);
    }

	
}
void
PriorityThread10(int param)
{
    // currentThread->Yield();
     printf("This is PriorityThread10\n");
     plocktest4->Acquire();
     cond->Wait(plocktest4);
     plocktest4->Release();
     plocktest3->Acquire();
     printf("L10:1 get lock\n");
    //currentThread->Yield();
    // printf("L10:2  back from yield\n");
     plocktest3->Release();
     printf("L10:3  release lock\n");	
}

void
PriorityTest3()
{
	DEBUG('t', "Entering PriorityTest1");

	plocktest3 = new Lock("plockTest3");
	plocktest4 = new Lock("plocktest4");
	cond = new Condition("cond");

	Thread *t = new Thread("one");
	Thread *x = new Thread("two");
	Thread *y = new Thread("Three");
	x->setPriority(10);
	y->setPriority(20);
	t->setPriority(1);
	t->Fork(PriorityThread8, 0);
	printf("the priority of t is: %d\n", t->getPriority());
	//x->setPriority(10);
	//y->setPriority(20);
	y->Fork(PriorityThread10, 0);
	//y->setPriority(20);
	printf("the priority of y is: %d\n", y->getPriority());
	x->Fork(PriorityThread9, 0);
	//x->setPriority(10);
	printf("the priority of x is: %d\n", x->getPriority());
	
	
}

//-------------------------------------------------------------------
// extraPrioritytest2
//-------------------------------------------------------------------
Lock *plocktest5 = NULL;

void
extraPriorityThread5(int param)
{
     plocktest5->Acquire();
     printf("L5:1 get lock\n");
//     currentThread->Yield();

     plocktest5->Release();
     printf("L5:3  release lock\n");	
}

void
extraPriorityThread6(int param)
{
int num;

  for (num = 0; num < 10; num++) {
    printf("PriorityThread9 looped %d times\n",num);
    }
     	
     currentThread->Yield();
}

void
extraPriorityThread7(int param)
{    
Thread *y=new Thread("Three"); 
Thread *x=new Thread("Two");
     plocktest5->Acquire();
     printf("L7:1 get lock\n");
        y->setPriority(15);
	y->Fork(extraPriorityThread5, 0);
	printf("the priority of y is: %d\n", y->getPriority());
        currentThread->Yield();
	printf("the priority of Thread7 is: %d\n", currentThread->getPriority());
	x->setPriority(10);
      	x->Fork(extraPriorityThread6, 0);
	printf("the priority of x is: %d\n", x->getPriority());
	currentThread->Yield();

     printf("L7:2  back from yield\n");
     plocktest5->Release();
     printf("L7:3  release lock\n");	
	
     printf("the priority of Thread7 is: %d\n", currentThread->getPriority());
}


void
extraPriorityTest2()
{
	DEBUG('t', "Entering extraPriorityTest5");

	plocktest5 = new Lock("LockTest5");

//	Thread *t = new Thread("one");
	Thread *t = new Thread("One");

//        t->setPriority(-6);
//	printf("the priority of t is: %d\n", t->getPriority());
//	t->Fork(PriorityThread4, 0);

	t->setPriority(6);
	t->Fork(extraPriorityThread7, 0);
	printf("the priority of z is: %d\n", t->getPriority());

}
//
//
//
void
pJoiner1(Thread *joinee1)
{
    int num;
    printf("Parent thread is going to run as T1\n");

    for (num = 0; num < 7; num++) {
        printf("T1 is now working at line %d\n", num);
        currentThread->Yield();
    }
    currentThread->Yield();
    currentThread->Yield();
    currentThread->Yield();
    currentThread->Yield();
    
    joinee1->Join();
    printf("Joinee has finish executing, we can continue\n");
}

void
pJoinee1()
{
    int num;
    printf("Child thread is going to run as T2\n");

    for (num = 0; num < 2; num++) {
        printf("T2 is now working at line %d\n", num);
        currentThread->Yield();
    }
    printf("Child Thread has finished work,wait for Join\n");
}

void
extrajPriorityThread(int param)
{
int num;

  for (num = 0; num < 10; num++) {
    printf("PriorityThread9 looped %d times\n",num);
    }
     	
}
void
PriorityJoinThread1() {
  Thread *joiner1 = new Thread("joiner1", 0);  // will not be joined
  Thread *joinee1 = new Thread("joinee1", 1);  // WILL be joined
  Thread *t = new Thread("One");
  joiner1->setPriority(20);
  // fork off the two threads and let them do their business
  joiner1->Fork((VoidFunctionPtr) pJoiner1, (int) joinee1);
  joinee1->Fork((VoidFunctionPtr) pJoinee1, 0);
  printf("the priority of x is: %d\n", joiner1->getPriority());
  printf("the priority of x is: %d\n", joinee1->getPriority());
  t->setPriority(10);
  t->Fork(extrajPriorityThread,0);
  
  //       // this thread is done and can go on its merry way
   printf("Forked off the joiner and joiner threads.\n");
}



//---------------------------------------------------------------------
//Mating Whales Test
//---------------------------------------------------------------------

Whale *Whale1;

void WhalesThread1(int param)
{

    Whale1->Male();

}

void WhalesThread2(int param)
{
     Whale1->Female();

}

void WhalesThread3(int param)
{
    Whale1->Matchmaker();
}

//Mating Whales Test1: single male, female, matchmaker
//Expected output(s):
//Thread male1 and Thread female1 mate successfully.

void
WhalesTest1(){
    
    DEBUG('t', "Entering WhalesTest1");
    Whale1 = new Whale("WhalesTest1");
    Thread *T1 = new Thread("male1");
    T1->Fork(WhalesThread1, 0);
    Thread *T2 = new Thread("female1");
    T2->Fork(WhalesThread2, 0);
    Thread *T3 = new Thread("matchmaker1");
    T3->Fork(WhalesThread3, 0);
  
}

//Mating Whales Test2: multiple of, male female, matchmaker
//Expected output(s): 
//Thread male1 and Thread female1 mate successfully.
//Thread male2 and Thread female2 mate successfully.
//Thread male3 and Thread female3 mate successfully.

void
WhalesTest2(){
    
    DEBUG('t', "Entering WhalesTest2");
    Whale1 = new Whale("WhalesTest2");
    Thread *T1 = new Thread("male1");
    T1->Fork(WhalesThread1, 0);
    T1 = new Thread("male2");
    T1->Fork(WhalesThread1, 0);
    Thread *T2 = new Thread("female1");
    T2->Fork(WhalesThread2, 0);
    T1 = new Thread("male3");
    T1->Fork(WhalesThread1, 0);
    T2 = new Thread("female2");
    T2->Fork(WhalesThread2, 0);
    Thread *T3 = new Thread("matchmaker1");
    T3->Fork(WhalesThread3, 0);
    T2 = new Thread("female3");
    T2->Fork(WhalesThread2, 0);
    T3 = new Thread("matchmaker2");
    T3->Fork(WhalesThread3, 0);  
    T3 = new Thread("matchmaker3");
    T3->Fork(WhalesThread3, 0);  }


//Mating Whales Test3: unequal amount of male, female, matchmaker
//Expected output(s): 
//Thread male1 and Thread female1 mate successfully.

void
WhalesTest3(){
    
    DEBUG('t', "Entering WhalesTest3");
    Whale1 = new Whale("WhalesTest3");
    Thread *T1 = new Thread("male1");
    T1->Fork(WhalesThread1, 0);
    Thread *T2 = new Thread("female1");
    T2->Fork(WhalesThread2, 0);
    T1 = new Thread("male2");
    T1->Fork(WhalesThread1, 0);
    T1 = new Thread("male3");
    T1->Fork(WhalesThread1, 0);
    T2 = new Thread("female2");
    T2->Fork(WhalesThread2, 0);
    Thread *T3 = new Thread("matchmaker1");
    T3->Fork(WhalesThread3, 0);
    T2 = new Thread("female3");
    T2->Fork(WhalesThread2, 0); 
  
}



//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest(){
    switch (testnum) {
    case 1:
        ThreadTest1();
        break;
    case 2:
        LockTest1();
        break;
    case 3:
        ConditionVariablesTest1();
        break;
   case 11:
        LockTest11();
        break;
    case 12:
        LockTest12();
        break;
    case 13:
        LockTest13();
        break;
    case 14:
        ConditionVariablesTest14();
        break;
    case 15:
        ConditionVariablesTest15();
        break;
    case 16:
        ConditionVariablesTest16();
        break;
    case 17:
        ConditionVariablesTest17();
        break;
    case 18:
        ConditionVariablesTest18();
        break;
    case 21:
        MailboxTest1();
        break;
    case 22:
        MailboxTest2();
        break;
    case 23:
        MailboxTest3();
        break;
    case 31:
        JoinThread1();   //child will wait when not being called even if it finishes
        break;
    case 32:
        JoinThread2();  //a parent calls Join on a child and the child is still executing, the parents waits
        break;
    case 33:
        JoinThread3();  //   if a parent calls Join on a child and the child has finished executing, the parent does not stop
        break;
     case 34:
        JoinThread4();  // a thread can not Join itself
        break;
     case 35:
        JoinThread5();  // a thread can only Join once
        break;
     case 36:
        JoinThread6();  // a thread can only Join once
        break;
     case 37:
        JoinThread7();  // a thread can only Join once
        break;
     case 41:
	PriorityTest1();
	break;
    case 42:
        PriorityTest2();
	break;
    case 43:
	PriorityTest3();
	break;
    case 44:
	extraPriorityTest2();
	break;
     case 45:
	PriorityJoinThread1();
	break;
     case 51:
        WhalesTest1();
        break;
    case 52:
        WhalesTest2();
        break;
    case 53:
        WhalesTest3();
        break;
   case 62:
	break;
    case 60:
        WhalesTest3();
         break;
     default:
        printf("No test specified.\n");
        break;
    }
}

