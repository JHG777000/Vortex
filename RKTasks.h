/*
 Copyright (c) 2016 Jacob Gordon. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: RKTasks.h
//Header file for RKTasks.

#ifndef RKTasks_h
#define RKTasks_h

#include <stdatomic.h>

typedef struct RKThread_s RKThread ;

typedef RKThread* RKThreads ;

typedef struct RKT_Lock_s RKT_Lock ;

typedef struct RKTasks_Task_s* RKTasks_Task ;

typedef struct RKTasks_ThisTask_s* RKTasks_ThisTask ;

typedef struct RKTasks_TaskGroup_s* RKTasks_TaskGroup ;

typedef struct RKTasks_ThreadGroup_s* RKTasks_ThreadGroup ;

#define RKTasks_StartLock( lock ) RKTasks_InitLock( &(lock) )

#define RKTasks_EndLock( lock ) RKTasks_KillLock( &(lock) )

#define RKTasks_LockLock( lock ) RKTasks_CloseLock( &(lock) )

#define RKTasks_UnLockLock( lock ) RKTasks_OpenLock( &(lock) )

int RKTasks_InitLock( RKT_Lock* lock ) ;

int RKTasks_KillLock( RKT_Lock* lock ) ;

int RKTasks_CloseLock( RKT_Lock* lock ) ;

int RKTasks_OpenLock( RKT_Lock* lock ) ;

typedef struct { int refcount ; } TaskArgs_Type ;

typedef TaskArgs_Type* TaskArgs_Type_ptr ;

#define RKTasks_CreateTask(TaskName, Args) typedef struct {int refcount ; Args} TaskName ## _argstruct ; \
typedef TaskName ## _argstruct* TaskName ## _argstruct_ptr ;\
static void TaskName##_TaskFunc(TaskName ## _argstruct_ptr RKTArgs, RKTasks_ThisTask ThisTask) ;\
static void TaskName( void *argument, RKTasks_ThisTask ThisTask ) { \
TaskName ## _argstruct_ptr RKTArgs = (TaskName ## _argstruct_ptr) argument ; \
\
TaskName##_TaskFunc(RKTArgs,ThisTask) ;\
\
}\
static void TaskName##_TaskFunc(TaskName ## _argstruct_ptr RKTArgs, RKTasks_ThisTask ThisTask)\

#define RKTasks_Args( TaskName ) TaskName ## _argstruct_ptr TaskName ## _Args = NULL

#define RKTasks_UseArgs( TaskName ) TaskName ## _Args = RKMem_CArray(1, TaskName ## _argstruct);\
((TaskArgs_Type_ptr)TaskName ## _Args)->refcount = 0

#define RKTasks_AddTask(TaskGroup, TaskFunc, TaskArgs) RKTasks_AddTask_Func(TaskGroup, TaskFunc, (void*)TaskArgs)

/*
 RKTasks_GetNumberOfThreadsForPlatform
 Computes a number, that is the "ideal" number of threads based on the number of cores and additional info supplied
 
 dynamic0No1Yes: if set to 1(or nonzero) will use sysconf( _SC_NPROCESSORS_ONLN ) to find the number of cores, if 0 it will use userSetNumOfCores
 as the number of cores.
 
 mode: Determine how the number of threads is computed.
 
 mode 1: Set the number of threads to min_num_of_threads.
 
 modes: 2 - 6, based on the a thread is expensive idea, so they are largely designed to lower the amount of threads made, in other words less is better.
 
 mode 7: Set the number of threads to number of cores(userSetNumOfCores or whatever sysconf returned).
 
 mode 8: Set the number of threads to max_num_of_threads.
 
 */

int RKTasks_GetNumberOfThreadsForPlatform( int min_num_of_threads, int max_num_of_threads, int userSetNumOfCores, int dynamic0No1Yes, int mode ) ;

/*
 RKTasks_NewTaskGroup
 
 Creates a task group.
 
 */

RKTasks_TaskGroup RKTasks_NewTaskGroup( void ) ;

/*
 RKTasks_KillTaskGroup
 
 Deletes a task group.
 
 Note: Does not check if the task group is done, should use RKTasks_WaitForTasksToBeDone before.
 
 */

void RKTasks_KillTaskGroup( RKTasks_TaskGroup TaskGroup ) ;

/*
 RKTasks_NewThreadGroup
 
 Creates a thread group, and sets the number of threads that should be made using RKTasks_GetNumberOfThreadsForPlatform.
 
 In most cases one thread group should be enough.
 
 */

RKTasks_ThreadGroup RKTasks_NewThreadGroup( int min_num_of_threads, int max_num_of_threads, int userSetNumOfCores, int dynamic0No1Yes, int mode ) ;

/*
 RKTasks_KillThreadGroup
 
 Kills threads first, using RKTasks_KillAllThreads.
 
 */

void RKTasks_KillThreadGroup( RKTasks_ThreadGroup ThreadGroup ) ;

/*
 RKTasks_BindTaskGroupToThreadGroup
 
 The bound TaskGroup will be run by the ThreadGroup it is bound to.
 
 A ThreadGroup can only have one TaskGroup bound to it, at a time.
 
 A TaskGroup can be bound to more than one thread group.
 
 */

void RKTasks_BindTaskGroupToThreadGroup(RKTasks_TaskGroup TaskGroup, RKTasks_ThreadGroup ThreadGroup) ;

/*
 RKTasks_SpawnThreads
 
 Spawns threads via pthreads.
 
 WARNING: This function has no mutexs or locks. It is up to the calling function to make
 sure data is locked.
 
 */

int RKTasks_SpawnThreads(RKTasks_ThreadGroup ThreadGroup) ;

/*
 RKTasks_RunThreadGroup
 
 Activates a thread group, and sapwns threads via RKTasks_SpawnThreads.
 
 */

void RKTasks_RunThreadGroup( RKTasks_ThreadGroup ThreadGroup ) ;

/*
 RKTasks_StopThreadGroup
 
 Deactivates a thread group, does not end threads.
 Threads will not wake when their thread group is not active.
 Threads will not be put to sleep if already running.
 
 */

void RKTasks_StopThreadGroup( RKTasks_ThreadGroup ThreadGroup ) ;

/*
 RKTasks_UseTaskGroup
 
 Must be called before each time a TaskGroup is to run.
 Failure to do so may lead to halting of the calling thread of RKTasks_WaitForTasksToBeDone.
 
 */

void RKTasks_UseTaskGroup( RKTasks_TaskGroup TaskGroup ) ;

/*
 RKTasks_GetNumOfThreads
 
 Returns the total number of threads that have been created.
 
 */

int RKTasks_GetNumOfThreads( RKTasks_ThreadGroup ThreadGroup) ;

/*
 RKTasks_GetNumOfTasks
 
 Returns the current number of tasks.
 
 */

int RKTasks_GetNumOfTasks( RKTasks_TaskGroup TaskGroup) ;

/*
 RKTasks_AllTasksDone
 
 Returns 1, if all tasks of the given TaskGroup are done, if not
 then returns zero.
 
 */

int RKTasks_AllTasksDone( RKTasks_TaskGroup TaskGroup ) ;

/*
 RKTasks_WaitForTasksToBeDone
 
 Will halt the calling thread until all tasks in the given TaskGroup are done.
 
 */

void RKTasks_WaitForTasksToBeDone( RKTasks_TaskGroup TaskGroup ) ;

/*
 RKTasks_AllThreadsDead
 
 Returns 1, if all threads of the given ThreadGroup are dead, if not
 then returns zero.
 
 */

int RKTasks_AllThreadsDead( RKTasks_ThreadGroup ThreadGroup ) ;

/*
 RKTasks_KillAllThreads
 
 Will halt the calling thread until all threads in the given ThreadGroup are dead.
 
 Threads will be allowed to live until they go to sleep.
 
 */

void RKTasks_KillAllThreads( RKTasks_ThreadGroup ThreadGroup ) ;

/*
 RKTasks_KillThreadWithTid
 
 Will halt the calling thread until the thread with the given tid is dead.
 
 The thread will be allowed to live until it goes to sleep.
 
 -1 retval means given tid was either less than zero or greater than the number of total threads made.
 
 */

int RKTasks_KillThreadWithTid( RKTasks_ThreadGroup ThreadGroup, int tid ) ;


/*
 RKTasks_DeactivateTask
 
 Deactivate given Task, via its ThisTask object.
 
 Task while unactive, will still be in memory and hold resources.
 
 */

void RKTasks_DeactivateTask( RKTasks_ThisTask ThisTask ) ;

/*
 RKTasks_GetTaskID
 
 Returns the Task ID.
 
 */

int RKTasks_GetTaskID( RKTasks_ThisTask ThisTask ) ;

/*
 RKTasks_GetThreadID
 
 Returns the Thread ID.
 
 */

int RKTasks_GetThreadID( RKTasks_ThisTask ThisTask ) ;

/*
 RKTasks_AddTask_Func
 
 Add a task with task args to the given TaskGroup. Can be used on a running TaskGroup.
 
 Threads will run new tasks as they get them.
 
 The macro RKTasks_AddTask is available which explicitly casts TaskArgs to (void*).
 
 Returns a ThisTask object. ThisTask is also passed to the task itself.
 
 */

RKTasks_ThisTask RKTasks_AddTask_Func(RKTasks_TaskGroup TaskGroup, void (*TaskFunc)(void *, struct RKTasks_ThisTask_s *), void *TaskArgs ) ;

///Atomics///

typedef atomic_int RKT_AtomicInt ;

void RKTasks_AtomicInc( RKT_AtomicInt* val ) ;

#endif /* RKTasks_h */

