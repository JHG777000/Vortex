/*
 Copyright (c) 2017 Jacob Gordon. All rights reserved.
 
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
#include <string.h>
#include <pthread.h>
#include "RKMath.h"
#include "RKTasks.h"

typedef struct RKThread_s RKThread ;

typedef RKThread* RKThreads ;

struct RKTasks_Module_s { RKTasks_ModuleDestructor_Type destructor ; pthread_mutex_t mutex ; void* data ; int refcount ; int tid ; int dead ; } ;

struct RKTasks_ThisTask_s { int active ; int delete ; int task_id ; int thread_id ; } ;

struct RKTasks_Task_s { RKTasks_ThisTask this_task ; int done ; int run_id ; int (*task_func)(RKTasks_Module, struct RKTasks_ThisTask_s *) ; RKTasks_Module module ; } ;

struct RKThread_s { pthread_t thread ; int alive ; int dead ; int awake ; int sleep ; } ;

struct RKTasks_TaskGroup_s { RKTasks_Task* task_array ; int run_id ; int num_of_tasks ; int bound ; int done ; int dead ; int active ;
    
pthread_cond_t done_tasks_cond ; pthread_mutex_t done_tasks_mutex ; RKMAtomicInt num_of_done_tasks ; RKMAtomicInt num_of_dead_tasks ; } ;

struct RKTasks_ThreadGroup_s { RKTasks_TaskGroup taskgroup ; RKThreads threadarray ; int max_num_of_threads ;
    
int num_of_threads ; int awake ; int alive ; int init ; } ;

typedef struct RKTasks_Thread_Args_s { RKTasks_ThreadGroup threadgroup ; int tid ; }* RKTasks_Thread_Args ;

RKTasks_Module RKTasks_NewModule( RKTasks_Module_NewDataFunc_Type module_newdatafunc, RKTasks_ModuleDestructor_Type moduledestructor) {
    
    RKTasks_Module module = RKMem_NewMemOfType(struct RKTasks_Module_s) ;
    
    module->data = module_newdatafunc() ;
    
    module->destructor = moduledestructor ;
    
    pthread_mutex_init(&(module->mutex), NULL) ;
    
    module->refcount = 0 ;
    
    module->tid = -2 ;
    
    module->dead = 0 ;
    
    return module ;
}

void RKTAsks_AddToModuleRefCount( RKTasks_Module module ) {
    
    module->refcount++ ;
}

void RKTasks_DestroyModule( RKTasks_Module module, int tid ) {
    
    RKTasks_LockModule(module) ;
    
    if ( module->tid == -2 ) module->tid = tid ;
    
    RKTasks_UnLockModule(module) ;
    
    if ( module->tid != tid ) return ;
    
    module->destructor(module) ;
    
    module->dead = 1 ;
    
    while ( RKTasks_IsModuleLocked(module) ) {}
    
    pthread_mutex_destroy(&(module->mutex)) ;
    
    free(module->data) ;
    
    free(module) ;
}

void RKTasks_DeleteModule( RKTasks_Module module, int tid ) {
    
    module->refcount-- ;
    
    if ( module->refcount <= 0 ) RKTasks_DestroyModule(module,tid) ;
    
}

int RKTasks_LockModule( RKTasks_Module module ) {
    
    if ( module->dead ) return -1 ;
    
    if ( pthread_mutex_trylock(&(module->mutex)) == 0 ) return 0 ;
    
    return 1 ;
}

int RKTasks_IsModuleLocked( RKTasks_Module module ) {
    
    if ( pthread_mutex_trylock(&(module->mutex)) ) {
        
        pthread_mutex_unlock(&(module->mutex)) ;
        
        return 0 ;
    }
    
    return 1 ;
}


int RKTasks_UnLockModule( RKTasks_Module module ) {
    
    return pthread_mutex_unlock(&(module->mutex))  ;
}

void* RKTasks_GetDataFromModule( RKTasks_Module module ) {

    return module->data ;

}

static int RKTasks_GetNumberOfThreadsForPlatform( int max_num_of_threads ) {
    
    int num_of_threads = (int) sysconf( _SC_NPROCESSORS_ONLN ) ;
    
    if ( num_of_threads > max_num_of_threads ) num_of_threads = max_num_of_threads ;
    
    return num_of_threads ;
}

RKTasks_TaskGroup RKTasks_NewTaskGroup( void ) {
    
    RKTasks_TaskGroup taskgroup = RKMem_NewMemOfType(struct RKTasks_TaskGroup_s) ;
    
    taskgroup->active = 1 ;
    
    taskgroup->dead = 0 ;
    
    taskgroup->done = 0 ;
    
    taskgroup->bound = 0 ;
    
    taskgroup->run_id = 0 ;
    
    taskgroup->num_of_tasks = 0 ;
    
    taskgroup->num_of_done_tasks = 0 ;
    
    taskgroup->num_of_dead_tasks = 0 ;
    
    taskgroup->task_array = NULL ;
    
    pthread_mutex_init(&taskgroup->done_tasks_mutex, NULL) ;
    
    pthread_cond_init(&taskgroup->done_tasks_cond, NULL);
    
    return taskgroup ;
}

static void RKTasks_DestroyTask( RKTasks_Task task, int tid ) {
    
    free(task->this_task) ;
    
    RKTasks_DeleteModule(task->module, tid) ;
    
    free(task) ;
}

void RKTasks_DestroyTaskGroup( RKTasks_TaskGroup taskgroup, int tid ) {
    
    int i = 0 ;
    
    while (i < taskgroup->num_of_tasks) {
        
        RKTasks_DestroyTask(taskgroup->task_array[i], tid) ;

        i++ ;
    }

    pthread_mutex_destroy(&taskgroup->done_tasks_mutex) ;
    
    pthread_cond_destroy(&taskgroup->done_tasks_cond) ;
    
    free(taskgroup->task_array) ;
    
    free(taskgroup) ;
}

void RKTasks_SetTaskGroupActive( RKTasks_TaskGroup taskgroup, int active ) {
    
    if ( active ) taskgroup->active = 1 ;
    
    if ( !active ) taskgroup->active = 0 ;
}

int RKTasks_IsTaskGroupActive( RKTasks_TaskGroup taskgroup ) {
    
    return taskgroup->active ;
}

int RKTasks_IsTaskGroupDone( RKTasks_TaskGroup taskgroup ) {
    
    return taskgroup->done ;
}

int RKTasks_IsTaskGroupDead( RKTasks_TaskGroup taskgroup ) {
    
    return taskgroup->dead ;
}

void RKTasks_ResetTaskGroup( RKTasks_TaskGroup taskgroup ) {
    
    pthread_mutex_lock(&taskgroup->done_tasks_mutex) ;
    
    if ( !taskgroup->done ) {
        
        pthread_mutex_unlock(&taskgroup->done_tasks_mutex) ;
        
        return ;
    }
    
    taskgroup->num_of_done_tasks = 0 ;
    
    taskgroup->run_id = !taskgroup->run_id ;
    
    taskgroup->done = 0 ;
    
    pthread_mutex_unlock(&taskgroup->done_tasks_mutex) ;
}

void RKTasks_BindTaskGroupToThreadGroup( RKTasks_TaskGroup taskgroup, RKTasks_ThreadGroup threadgroup ) {
    
    if ( !threadgroup->init ) return ;
    
    RKTasks_SleepThreadGroup(threadgroup) ;
    
    taskgroup->bound = 1 ;
    
    threadgroup->taskgroup = taskgroup ;
    
    RKTasks_AwakeThreadGroup(threadgroup) ;
}

void RKTasks_UnbindTaskGroupFromThreadGroup( RKTasks_TaskGroup taskgroup, RKTasks_ThreadGroup threadgroup ) {
    
    if ( !threadgroup->init || !taskgroup->bound ) return ;
    
    RKTasks_SleepThreadGroup(threadgroup) ;

    taskgroup->bound = 0 ;
    
    threadgroup->taskgroup = NULL ;
    
    RKTasks_AwakeThreadGroup(threadgroup) ;
}

void RKTasks_AddTasks_Func( RKTasks_TaskGroup taskgroup, int num_of_tasks, int (*task_func)(RKTasks_Module, struct RKTasks_ThisTask_s *), RKTasks_Module module ) {
    
    int i = 0 ;
    
    RKTasks_Task task = NULL ;
    
    RKTasks_ThisTask thistask = NULL ;
    
    if ( taskgroup->bound ) return ;
    
    taskgroup->num_of_tasks += num_of_tasks ;
    
    if ( taskgroup->task_array == NULL ) {
        
        taskgroup->task_array = RKMem_CArray(taskgroup->num_of_tasks, RKTasks_Task) ;
        
    } else {
        
        taskgroup->task_array = RKMem_Realloc(taskgroup->task_array, taskgroup->num_of_tasks, taskgroup->num_of_tasks-num_of_tasks, RKTasks_Task, 1) ;
    }
    
    i = taskgroup->num_of_tasks-num_of_tasks ;
    
    while ( i < taskgroup->num_of_tasks ) {
        
        thistask = RKMem_NewMemOfType(struct RKTasks_ThisTask_s) ;
        
        thistask->active = 1 ;
        
        thistask->delete = 0 ;
        
        thistask->thread_id = -3 ;
        
        thistask->task_id = i ;
        
        task = RKMem_NewMemOfType(struct RKTasks_Task_s) ;
        
        task->done = 0 ;
        
        task->run_id = 0 ;
        
        task->this_task = thistask ;
        
        task->task_func = task_func ;
        
        RKTAsks_AddToModuleRefCount(module) ;
        
        task->module = module ;
        
        taskgroup->task_array[i] = task ;

        i++ ;
    }
    
}

static void RKTasks_DeadDoneCheck( int* dead, int* done, int num_of_tasks, RKTasks_TaskGroup taskgroup ) {
    
    pthread_mutex_lock(&taskgroup->done_tasks_mutex) ;
    
    if ( taskgroup->num_of_dead_tasks == num_of_tasks ) *dead = 1 ;
    
    if ( taskgroup->num_of_done_tasks == num_of_tasks ) *done = 1 ;
    
    pthread_mutex_unlock(&taskgroup->done_tasks_mutex) ;
}

static void *RKTasks_WorkerThread( void *argument ) {
    
    RKTasks_Thread_Args threadargs = argument ;
    
    RKTasks_ThreadGroup threadgroup = threadargs->threadgroup ;
    
    RKTasks_TaskGroup taskgroup = NULL ;
    
    RKTasks_Task task = NULL ;
    
    int tid = threadargs->tid ;
    
    int index = threadargs->tid ;
    
    while ( threadgroup->alive && threadgroup->threadarray[tid].alive ) {
        
        while ( threadgroup->awake && threadgroup->threadarray[tid].awake ) {
            
            if ( !threadgroup->alive || !threadgroup->threadarray[tid].alive ) break ;
            
            threadgroup->threadarray[tid].sleep = 0 ;
            
            if ( (taskgroup != threadgroup->taskgroup) && (threadgroup->taskgroup != NULL) ) taskgroup = threadgroup->taskgroup ;
            
            if ( (taskgroup == NULL) || (taskgroup->task_array == NULL) ) continue ;
            
            if ( (taskgroup->active) && (!taskgroup->done) && (!taskgroup->dead)) {
            
              task = (index >= taskgroup->num_of_tasks) ? NULL : taskgroup->task_array[index] ;
            
              if ( task != NULL ) {
                  
                if ( (task->this_task->active) && (task->run_id == taskgroup->run_id) ) {
                
                    task->this_task->thread_id = tid ;
                  
                    task->done = task->task_func(task->module,task->this_task) ;
                    
                    if ( task->done ) {
                        
                        task->run_id = !task->run_id ;
                        
                        RKMath_AtomicInc(&taskgroup->num_of_done_tasks) ;
                    }
                    
                    if ( task->this_task->delete ) {
                        
                        RKTasks_DestroyTask(task,tid) ;
                        
                        taskgroup->task_array[index] = NULL ;
                        
                        RKMath_AtomicInc(&taskgroup->num_of_dead_tasks) ;
                    }
                    
                }
            
              }
            
              index += threadgroup->num_of_threads ;

            }
            
            if ( index >= taskgroup->num_of_tasks ) {
                
                index = tid ;
                
                RKTasks_DeadDoneCheck(&taskgroup->dead, &taskgroup->done, taskgroup->num_of_tasks, taskgroup) ;
                
                if ( (taskgroup->done) || (taskgroup->dead) ) pthread_cond_signal(&taskgroup->done_tasks_cond) ;
            }
        }
        
        threadgroup->threadarray[tid].sleep = 1 ;
    }
    
    threadgroup->threadarray[tid].dead = 1 ;
    
    free(threadargs) ;
    
    return NULL ;
}

static int RKTasks_SpawnThreads( RKTasks_ThreadGroup threadgroup ) {
    
    RKTasks_Thread_Args threadargs = NULL ;
    
    int Error = 0 ;
    
    int t = 0 ;
    
    threadgroup->threadarray = RKMem_CArray(threadgroup->max_num_of_threads, RKThread) ;
    
    while ( t < threadgroup->max_num_of_threads ) {
        
        threadargs = RKMem_NewMemOfType(struct RKTasks_Thread_Args_s) ;
        
        threadargs->threadgroup = threadgroup ;
        
        threadargs->tid = t ;
        
        threadgroup->threadarray[t].alive = 1 ;
        
        threadgroup->threadarray[t].dead = 0 ;
        
        threadgroup->threadarray[t].awake = 1 ;
        
        threadgroup->threadarray[t].sleep = 0 ;
        
        Error = pthread_create(&(threadgroup->threadarray[t].thread), NULL, RKTasks_WorkerThread, (void *) threadargs) ;
        
        if ( Error ) return Error ;
        
        Error = pthread_detach( threadgroup->threadarray[t].thread ) ;
        
        if ( Error ) return Error ;
        
        t++ ;
        
    }
    
    threadgroup->num_of_threads = t ;
    
    threadgroup->init = 1 ;
    
    return Error ;
    
}

void RKTasks_StartThreadGroup( RKTasks_ThreadGroup threadgroup ) {
    
    if ( !threadgroup->init ) RKTasks_SpawnThreads(threadgroup) ;
}


RKTasks_ThreadGroup RKTasks_NewThreadGroup( int max_num_of_threads ) {
    
    RKTasks_ThreadGroup threadgroup = RKMem_NewMemOfType(struct RKTasks_ThreadGroup_s) ;
    
    threadgroup->threadarray = NULL ;
    
    threadgroup->max_num_of_threads = RKTasks_GetNumberOfThreadsForPlatform(max_num_of_threads) ;
    
    threadgroup->num_of_threads = 0 ;
    
    threadgroup->taskgroup = NULL ;
    
    threadgroup->awake = 0 ;
    
    threadgroup->alive = 1 ;
    
    threadgroup->init = 0 ;
    
    return threadgroup ;
}

void RKTasks_DestroyThreadGroup( RKTasks_ThreadGroup threadgroup ) {
    
    RKTasks_KillAllThreads(threadgroup) ;
    
    free(threadgroup->threadarray) ;
    
    threadgroup->threadarray = NULL ;
    
    free(threadgroup) ;
}

void RKTasks_WaitForTasksToBeDone( RKTasks_TaskGroup taskgroup ) {
    
    if ( taskgroup->num_of_tasks == 0 ) return ;
    
    if ( (taskgroup->done) || (taskgroup->dead) ) return ;
    
    pthread_mutex_lock(&taskgroup->done_tasks_mutex) ;
    
    pthread_cond_wait(&taskgroup->done_tasks_cond, &taskgroup->done_tasks_mutex) ;
    
    pthread_mutex_unlock(&taskgroup->done_tasks_mutex) ;
}

int RKTasks_AllThreadsDead( RKTasks_ThreadGroup threadgroup ) {
    
    int i = 0 ;
    
    while ( i < threadgroup->num_of_threads ) {
        
        if ( !threadgroup->threadarray[i].dead ) return 0 ;
        
        i++ ;
    }
    
    return 1 ;
}

void RKTasks_KillAllThreads( RKTasks_ThreadGroup ThreadGroup ) {
    
    ThreadGroup->alive = 0 ;
    
    while ( 1 ) {
        
        if ( RKTasks_AllThreadsDead(ThreadGroup) ) break ;
    }
    
}

int RKTasks_KillThreadWithTid( RKTasks_ThreadGroup threadgroup, int tid ) {
    
    if ( (tid < 0) || (tid > threadgroup->num_of_threads) ) return -1 ;
    
    threadgroup->threadarray[tid].alive = 0 ;
    
    while ( 1 ) {
        
        if ( threadgroup->threadarray[tid].dead == 1 ) break ;
    }
    
    return 0 ;
}

int RKTasks_AllThreadsSleep( RKTasks_ThreadGroup threadgroup ) {
    
    int i = 0 ;
    
    while ( i < threadgroup->num_of_threads ) {
        
        if ( !threadgroup->threadarray[i].sleep ) return 0 ;
        
        i++ ;
    }
    
    return 1 ;
}

void RKTasks_SleepThreadGroup( RKTasks_ThreadGroup threadgroup ) {
    
    threadgroup->awake = 0 ;
    
    while ( 1 ) {
        
        if ( RKTasks_AllThreadsSleep(threadgroup) ) break ;
    }
    
}

int RKTasks_SleepThreadWithTid( RKTasks_ThreadGroup threadgroup, int tid ) {
    
    if ( (tid < 0) || (tid > threadgroup->num_of_threads) ) return -1 ;
    
    threadgroup->threadarray[tid].awake = 0 ;
    
    while ( 1 ) {
        
        if ( threadgroup->threadarray[tid].sleep == 1 ) break ;
    }
    
    return 0 ;
}

int RKTasks_AllThreadsAwake( RKTasks_ThreadGroup threadgroup ) {
    
    int i = 0 ;
    
    while ( i < threadgroup->num_of_threads ) {
        
        if ( threadgroup->threadarray[i].sleep ) return 0 ;
        
        i++ ;
    }
    
    return 1 ;
}

void RKTasks_AwakeThreadGroup( RKTasks_ThreadGroup threadgroup ) {
    
    threadgroup->awake = 1 ;
}

int RKTasks_IsThreadGroupAwake( RKTasks_ThreadGroup threadgroup ) {
    
    return threadgroup->awake ;
}

int RKTasks_GetNumOfThreads( RKTasks_ThreadGroup threadgroup ) {
    
    return threadgroup->num_of_threads ;
}

int RKTasks_AwakeThreadWithTid( RKTasks_ThreadGroup threadgroup, int tid ) {
    
    if ( (tid < 0) || (tid > threadgroup->num_of_threads) ) return -1 ;
    
    threadgroup->threadarray[tid].awake = 1 ;
    
    while ( 1 ) {
        
        if ( threadgroup->threadarray[tid].sleep == 0 ) break ;
    }
    
    return 0 ;
}

void RKTasks_DeactivateTask( RKTasks_ThisTask thistask ) {
    
    thistask->active = 0 ;
}

void RKTasks_DeleteTask( RKTasks_ThisTask thistask ) {
    
    thistask->delete = 1 ;
}

int RKTasks_GetTaskID( RKTasks_ThisTask thistask ) {
    
    return thistask->task_id ;
}

int RKTasks_GetThreadID( RKTasks_ThisTask thistask ) {
    
    return thistask->thread_id ;
}

