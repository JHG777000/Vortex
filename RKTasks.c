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

//File: RKTasks.c
//RKTasks, a thread pool library.

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "RKMem.h"
#include "RKTasks.h"

typedef RKList RKTasks_TaskList ;

typedef RKList_node RKTasks_Tasklet ;

struct RKT_Lock_s { pthread_mutex_t mutex ; } ;

struct RKThread_s { pthread_t thread ; int alive ; } ;

struct RKTasks_ThisTask_s { int kill ; int task_id ; int thread_id ; } ;

struct RKTasks_Task_s { RKT_Lock task_lock ; RKTasks_ThisTask ThisTask ; int active ;
    
void (*TaskFunc)(void *, struct RKTasks_ThisTask_s *) ; void *TaskArgs ; RKTasks_Tasklet list_node ; int task_run_id ; } ;

struct RKTasks_TaskGroup_s { RKT_Lock task_group_lock ; pthread_cond_t done_tasks_cond ; RKT_Lock done_tasks_lock ;
    
RKTasks_TaskList TaskList ; int NumOfDoneTasks ; int NumOfTasks ; int init ;
    
int group_run_id ; } ;

struct RKTasks_ThreadGroup_s { RKT_Lock thread_group_lock ; RKThreads ThreadArray ; int MaxNumOfThreads ; int NumOfDeadThreads ;
    
int NumOfThreads ; RKTasks_TaskGroup Current_TaskGroup ; int alive ; int GoToWork ; } ;

typedef struct { RKTasks_ThreadGroup ThreadGroup ; int tid ; } RKTasks_Thread_Args_object ;

typedef RKTasks_Thread_Args_object* RKTasks_Thread_Args ;

int RKTasks_InitLock( RKT_Lock* lock ) {
    
    return pthread_mutex_init(&(lock->mutex), NULL) ;
}

int RKTasks_KillLock( RKT_Lock* lock ) {
    
    return pthread_mutex_destroy(&(lock->mutex)) ;
}

int RKTasks_CloseLock( RKT_Lock* lock ) {
    
    return pthread_mutex_lock(&(lock->mutex)) ;
}

int RKTasks_OpenLock( RKT_Lock* lock ) {
    
    return pthread_mutex_unlock(&(lock->mutex)) ;
}

int RKTasks_GetNumberOfThreadsForPlatform( int min_num_of_threads, int max_num_of_threads, int userSetNumOfCores, int dynamic0No1Yes, int mode ) {
    
    int num_of_cores = 0 ;
    
    int num_of_threads = 0 ;
    
    min_num_of_threads = (min_num_of_threads <= 0) ? 1 : min_num_of_threads ;
    
    mode = (mode <= 0) ? 1 : mode ;
    
    mode = (mode >= 9) ? 8 : mode ;
    
    if ( dynamic0No1Yes ) {
        
        num_of_cores = (int) sysconf( _SC_NPROCESSORS_ONLN ) ;
        
    } else {
        
        num_of_cores = userSetNumOfCores ;
        
    }
    
    if ( mode == 1 ) {
        
        num_of_threads = min_num_of_threads ;
    }
    
    if ( mode == 2 ) {
        
        num_of_threads = ( min_num_of_threads + max_num_of_threads ) / 2 ;
    }
    
    if ( mode == 3 ) {
        
        if ( num_of_cores >= max_num_of_threads ) {
            
            while ( num_of_cores >= max_num_of_threads ) {
                num_of_cores = num_of_cores - 2 ;
            }
            
        } else {
            
            num_of_cores = num_of_cores - 2 ;
            
        }
        
        num_of_threads = num_of_cores ;
    }
    
    if ( mode == 4 ) {
        
        if ( num_of_cores >= max_num_of_threads ) {
            
            while ( num_of_cores >= max_num_of_threads ) {
                num_of_cores = num_of_cores / 2 ;
            }
            
        } else {
            
            num_of_cores = num_of_cores / 2 ;
            
        }
        
        num_of_threads = num_of_cores ;
    }
    
    if ( mode == 5 ) {
        
        num_of_threads = ( num_of_cores / 2 ) ;
        
    }
    
    
    if ( mode == 6 ) {
        
        num_of_threads = ( ( num_of_cores / 2 ) - 1 ) ;
        
    }
    
    if ( mode == 7 ) {
        
        num_of_threads = num_of_cores ;
        
    }
    
    if ( mode == 8 ) {
        
        num_of_threads = max_num_of_threads ;
        
    }
    
    num_of_threads = (num_of_threads < min_num_of_threads) ? min_num_of_threads : num_of_threads ;
    
    num_of_threads = (num_of_threads > max_num_of_threads) ? max_num_of_threads : num_of_threads ;
    
    return num_of_threads ;
    
}

RKTasks_TaskGroup RKTasks_NewTaskGroup( void ) {
    
    RKTasks_TaskGroup NewGroup = RKMem_NewMemOfType(RKTasks_TaskGroup_object) ;
    
    NewGroup->TaskList = RKList_NewList() ;
    
    NewGroup->NumOfTasks = 0 ;
    
    NewGroup->NumOfDoneTasks = 0 ;
    
    NewGroup->group_run_id = 0 ;
    
    NewGroup->init = 0 ;
    
    RKTasks_StartLock(NewGroup->task_group_lock) ;
    
    RKTasks_StartLock(NewGroup->done_tasks_lock) ;
    
    pthread_cond_init(&NewGroup->done_tasks_cond, NULL);
    
    return NewGroup ;
    
}

typedef struct { int refcount ; } TaskArgs_Type ;

typedef TaskArgs_Type* TaskArgs_Type_ptr ;

void RKTasks_KillTaskGroup( RKTasks_TaskGroup TaskGroup ) {
    
    RKTasks_Task Task = NULL ;
    
    RKTasks_Tasklet Tasklet = RKList_GetFirstNode(TaskGroup->TaskList) ;
    
    while ( Tasklet != NULL ) {
        
         Task = (RKTasks_Task)RKList_GetData(Tasklet) ;
        
         RKTasks_EndLock(Task->task_lock) ;
        
        if ( Task->TaskArgs != NULL ) {
        
         ((TaskArgs_Type_ptr)Task->TaskArgs)->refcount-- ;
        
         if ( ((TaskArgs_Type_ptr)Task->TaskArgs)->refcount <= 0 ) {
            
             free(Task->TaskArgs) ;
            
             Task->TaskArgs = NULL ;
         }
        
        }
        
        free(Task->ThisTask) ;
        
        free(Task) ;
        
        Tasklet = RKList_GetNextNode(Tasklet) ;
    }
    
    RKList_DeleteList(TaskGroup->TaskList) ;
    
    TaskGroup->TaskList = NULL ;
    
    TaskGroup->NumOfTasks = 0 ;
    
    TaskGroup->group_run_id = 0 ;
    
    RKTasks_EndLock(TaskGroup->task_group_lock) ;
    
    RKTasks_EndLock(TaskGroup->done_tasks_lock) ;
    
    free(TaskGroup) ;
}

RKTasks_ThreadGroup RKTasks_NewThreadGroup( int min_num_of_threads, int max_num_of_threads, int userSetNumOfCores, int dynamic0No1Yes, int mode ) {
    
    RKTasks_ThreadGroup NewGroup = RKMem_NewMemOfType(RKTasks_ThreadGroup_object) ;
    
    NewGroup->Current_TaskGroup = NULL ;
    
    NewGroup->ThreadArray = NULL ;
    
    NewGroup->MaxNumOfThreads = RKTasks_GetNumberOfThreadsForPlatform(min_num_of_threads,max_num_of_threads,userSetNumOfCores,dynamic0No1Yes,mode) ;
    
    NewGroup->NumOfDeadThreads = 0 ;
    
    NewGroup->NumOfThreads = 0 ;
    
    NewGroup->GoToWork = 0 ;
    
    NewGroup->alive = 1 ;
    
    RKTasks_StartLock(NewGroup->thread_group_lock) ;
    
    return NewGroup ;
    
}

void RKTasks_KillThreadGroup( RKTasks_ThreadGroup ThreadGroup ) {
    
    RKTasks_KillAllThreads(ThreadGroup) ;
    
    ThreadGroup->Current_TaskGroup = NULL ;
    
    free(ThreadGroup->ThreadArray) ;
    
    ThreadGroup->ThreadArray = NULL ;
    
    RKTasks_EndLock(ThreadGroup->thread_group_lock) ;
    
    free(ThreadGroup) ;
}

void RKTasks_BindTaskGroupToThreadGroup(RKTasks_TaskGroup TaskGroup, RKTasks_ThreadGroup ThreadGroup) {
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    ThreadGroup->Current_TaskGroup = TaskGroup ;
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
}

static void *RKTasks_WorkerThread( void *argument ) {
    
    RKTasks_Thread_Args ThreadArgs = (RKTasks_Thread_Args)argument ;
    
    RKTasks_ThreadGroup ThreadGroup = ThreadArgs->ThreadGroup ;
    
    RKTasks_TaskGroup TaskGroup = NULL ;
    
    RKTasks_Task Task = NULL ;
    
    RKTasks_Tasklet Tasklet = NULL ;
    
    int tid = ThreadArgs->tid ;
    
    int thread_run_state = 0 ;
    
    int not_found = 1 ;
    
    int alive = 1 ;
    
    int awake = 0 ;
    
    while (alive) {
        
        if ( ThreadGroup == NULL ) break ;
        
        RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
        
        if ( (!(ThreadGroup->alive)) || (!(ThreadGroup->ThreadArray[tid].alive))) alive = 0 ;
        
        if ( ThreadGroup->Current_TaskGroup != NULL ) {
            
            TaskGroup = ThreadGroup->Current_TaskGroup ;
            
            RKTasks_LockLock(TaskGroup->task_group_lock) ;
            
            if ( TaskGroup->init && ThreadGroup->GoToWork && TaskGroup->NumOfTasks > TaskGroup->NumOfDoneTasks ) {
                
                thread_run_state = TaskGroup->group_run_id ;
                
                awake = 1 ;
            }
            
            RKTasks_UnLockLock(TaskGroup->task_group_lock) ;
        }
        
        RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
        
        if (!alive) break ;
        
        while (awake) {
            
           if (Tasklet == NULL) Tasklet = RKList_GetFirstNode(TaskGroup->TaskList) ;
            
            while (not_found) {
                
                if ( Tasklet != NULL ) {
                    
                    Task = (RKTasks_Task)RKList_GetData(Tasklet) ;
                    
                    if ( Task->task_run_id == thread_run_state ) {
                        
                        RKTasks_LockLock(Task->task_lock) ;
                        
                        if ( Task->task_run_id == thread_run_state ) {
                            
                            Task->task_run_id = !(Task->task_run_id) ;
                            
                            Task->ThisTask->thread_id = tid ;
                            
                            not_found = 0 ;
                        }
                        
                        RKTasks_UnLockLock(Task->task_lock) ;
                        
                    } else {
                        
                        Tasklet = RKList_GetNextNode(Tasklet) ;
                    }
                    
                } else {
                    
                    awake = 0 ;
                    
                    break ;
                }
            }
            
            if ( !not_found ) {
                
                if ( Task->active ) {
                    
                    Task->TaskFunc(Task->TaskArgs,Task->ThisTask) ;
                    
                    if (Task->ThisTask->kill) Task->active = 0 ;
                    
                }
                
                RKTasks_LockLock(TaskGroup->done_tasks_lock) ;
                
                TaskGroup->NumOfDoneTasks++ ;
                
                if (TaskGroup->NumOfTasks <= TaskGroup->NumOfDoneTasks) pthread_cond_signal(&TaskGroup->done_tasks_cond) ;
                
                RKTasks_UnLockLock(TaskGroup->done_tasks_lock) ;
                
            }
            
            not_found = 1 ;
        }
    }
    
    if ( ThreadGroup != NULL ) {
        
        RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
        
        ThreadGroup->NumOfDeadThreads++ ;
        
        ThreadGroup->ThreadArray[tid].alive = -1 ;
        
        RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
        
    }
    
    free(ThreadArgs) ;
    
    return NULL ;
}

int RKTasks_SpawnThreads(RKTasks_ThreadGroup ThreadGroup) {
    
    RKTasks_Thread_Args ThreadArgs = NULL ;
    
    int Error = 0 ;
    
    int t = 0 ;
    
    ThreadGroup->ThreadArray = RKMem_CArray(ThreadGroup->MaxNumOfThreads, RKThread) ;
    
    while ( t < ThreadGroup->MaxNumOfThreads ) {
        
        ThreadArgs = RKMem_NewMemOfType(RKTasks_Thread_Args_object) ;
        
        ThreadArgs->ThreadGroup = ThreadGroup ;
        
        ThreadArgs->tid = t ;
        
        ThreadGroup->ThreadArray[t].alive = 1 ;
        
        Error = pthread_create(&(ThreadGroup->ThreadArray[t].thread), NULL, RKTasks_WorkerThread, (void *) ThreadArgs) ;
        
        if ( Error ) return Error ;
        
        Error = pthread_detach( ThreadGroup->ThreadArray[t].thread ) ;
        
        if ( Error ) return Error ;
        
        t++ ;
        
    }
    
    ThreadGroup->NumOfThreads = t ;
    
    return Error ;
    
}

void RKTasks_RunThreadGroup( RKTasks_ThreadGroup ThreadGroup ) {
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    if ( ThreadGroup->NumOfThreads == 0 ) RKTasks_SpawnThreads(ThreadGroup) ;
    
    if ( ThreadGroup->Current_TaskGroup != NULL ) ThreadGroup->GoToWork = 1 ;
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
}

void RKTasks_StopThreadGroup( RKTasks_ThreadGroup ThreadGroup ) {
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    if ( ThreadGroup->Current_TaskGroup != NULL ) ThreadGroup->GoToWork = 0 ;
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
}

void RKTasks_UseTaskGroup( RKTasks_TaskGroup TaskGroup ) {
    
    RKTasks_LockLock(TaskGroup->task_group_lock) ;
    
    if ( !(TaskGroup->init) ) {
        
        TaskGroup->init = 1 ;
        
    } else {
        
        TaskGroup->NumOfDoneTasks = 0 ;
        
        TaskGroup->group_run_id = !(TaskGroup->group_run_id) ;
        
    }
    
    RKTasks_UnLockLock(TaskGroup->task_group_lock) ;
}

int RKTasks_GetNumOfThreads( RKTasks_ThreadGroup ThreadGroup) {
    
    int num_of_threads = 0 ;
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    num_of_threads = ThreadGroup->NumOfThreads ;
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
    return num_of_threads ;
    
}

int RKTasks_GetNumOfTasks( RKTasks_TaskGroup TaskGroup) {
    
    int num_of_tasks = 0 ;
    
    RKTasks_LockLock(TaskGroup->task_group_lock) ;
    
    num_of_tasks = TaskGroup->NumOfTasks ;
    
    RKTasks_UnLockLock(TaskGroup->task_group_lock) ;
    
    return num_of_tasks ;
    
}

int RKTasks_AllTasksDone( RKTasks_TaskGroup TaskGroup ) {
    
    int num_of_tasks = 0 ;
    
    int done = 0 ;
    
    RKTasks_LockLock(TaskGroup->task_group_lock) ;
    
    num_of_tasks = TaskGroup->NumOfTasks ;
    
    done = TaskGroup->NumOfDoneTasks ;
    
    RKTasks_UnLockLock(TaskGroup->task_group_lock) ;
    
    if ( num_of_tasks <= done ) return 1 ;
    
    return 0 ;
}

void RKTasks_WaitForTasksToBeDone( RKTasks_TaskGroup TaskGroup ) {
    
    RKTasks_LockLock(TaskGroup->done_tasks_lock) ;
    
    pthread_cond_wait(&TaskGroup->done_tasks_cond, &TaskGroup->done_tasks_lock.mutex) ;
    
    RKTasks_UnLockLock(TaskGroup->done_tasks_lock) ;
}

int RKTasks_AllThreadsDead( RKTasks_ThreadGroup ThreadGroup ) {
    
    int num_of_threads = 0 ;
    
    int dead = 0 ;
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    num_of_threads = ThreadGroup->NumOfThreads ;
    
    dead = ThreadGroup->NumOfDeadThreads ;
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
    if ( num_of_threads == dead ) return 1 ;
    
    return 0 ;
}

void RKTasks_KillAllThreads( RKTasks_ThreadGroup ThreadGroup ) {
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    ThreadGroup->alive = 0 ;
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
    while ( 1 ) {
        
        if ( RKTasks_AllThreadsDead(ThreadGroup) ) break ;
    }
    
}

int RKTasks_KillThreadWithTid( RKTasks_ThreadGroup ThreadGroup, int tid ) {
    
    if ( (tid < 0) || (tid > ThreadGroup->NumOfThreads) ) return -1 ;
    
    RKTasks_LockLock(ThreadGroup->thread_group_lock) ;
    
    if ( ThreadGroup->ThreadArray[tid].alive != -1 ) {
        
        ThreadGroup->ThreadArray[tid].alive = 0 ;
        
    }
    
    RKTasks_UnLockLock(ThreadGroup->thread_group_lock) ;
    
    while ( 1 ) {
        
        if ( ThreadGroup->ThreadArray[tid].alive == -1 ) break ;
    }
    
    return 0 ;
}

void RKTasks_DeactivateTask( RKTasks_ThisTask ThisTask ) {
    
    ThisTask->kill = 1 ;
}

int RKTasks_GetTaskID( RKTasks_ThisTask ThisTask ) {
    
    return ThisTask->task_id ;
}

int RKTasks_GetThreadID( RKTasks_ThisTask ThisTask ) {
    
    return ThisTask->thread_id ;
}

RKTasks_ThisTask RKTasks_AddTask_Func(RKTasks_TaskGroup TaskGroup, void (*TaskFunc)(void *, struct RKTasks_ThisTask_s *), void *TaskArgs ) {
    
    TaskArgs_Type_ptr Args = TaskArgs ;
    
    Args->refcount++ ;
    
    RKTasks_ThisTask ThisTask = RKMem_NewMemOfType(RKTasks_ThisTask_object) ;
    
    ThisTask->kill = 0 ;
    
    RKTasks_Task Task = RKMem_NewMemOfType(RKTasks_Task_object) ;
    
    Task->active = 1 ;
    
    Task->ThisTask = ThisTask ;
    
    Task->TaskFunc = TaskFunc ;
    
    Task->TaskArgs = TaskArgs ;
    
    Task->task_run_id = TaskGroup->group_run_id ;
    
    RKTasks_StartLock(Task->task_lock) ;
    
    RKTasks_LockLock(TaskGroup->task_group_lock) ;
    
    Task->list_node = RKList_AddToList(TaskGroup->TaskList, (void*)Task) ;
    
    TaskGroup->NumOfTasks = RKList_GetNumOfNodes(TaskGroup->TaskList) ;
    
    Task->ThisTask->task_id = TaskGroup->NumOfTasks ;
    
    RKTasks_UnLockLock(TaskGroup->task_group_lock) ;
    
    return Task->ThisTask ;
    
}

