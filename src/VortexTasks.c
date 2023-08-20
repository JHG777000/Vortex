/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexTasks.c
//VortexTasks, a thread pool library.

#include <stdlib.h>

#if  !_WIN32
#include <unistd.h>
#endif

#include <string.h>
#include <Vortex/threads.h>
#include <Vortex/VortexTasks.h>

typedef struct VortexThread_s VortexThread;
typedef VortexThread* VortexThreads;

struct VortexTasks_Module_s {
  VortexTasks_ModuleDestructor_Type destructor;
  mtx_t mutex; 
  VortexAny data;
  vortex_ulong refcount;
  vortex_int tid;
  vortex_int dead; 
};
struct VortexTasks_ThisTask_s {
  vortex_int active;
  vortex_int delete;
  vortex_int task_id;
  vortex_int thread_id; 
};
struct VortexTasks_Task_s {
  VortexTasks_ThisTask this_task;
  vortex_int done;
  vortex_int run_id;
  vortex_int (*task_func)(VortexTasks_Module, struct VortexTasks_ThisTask_s*);
  VortexTasks_Module module;
};

struct VortexThread_s { 
  thrd_t thread; 
  vortex_int alive;
  vortex_int dead;
  vortex_int awake;
  vortex_int sleep;
  vortex_int num_of_done_tasks;
  vortex_int num_of_dead_tasks;
};

struct VortexTasks_TaskGroup_s {
  VortexTasks_Task* task_array;
  vortex_int run_id; vortex_ulong num_of_tasks;
  vortex_int bound; 
  vortex_int done; 
  vortex_int dead; 
  vortex_int active;
  cnd_t done_tasks_cond;
  mtx_t done_tasks_mutex;
  vortex_int num_of_done_tasks; 
  vortex_int num_of_dead_tasks;
};

struct VortexTasks_ThreadGroup_s {
  VortexTasks_TaskGroup taskgroup; 
  VortexThreads threadarray;
  vortex_int max_num_of_threads;
  vortex_int num_of_threads;
  vortex_int awake; 
  vortex_int alive;
  vortex_int init;
};

typedef struct VortexTasks_ThreadArgs_s {
  VortexTasks_ThreadGroup threadgroup;
  vortex_int tid;
}* VortexTasks_ThreadArgs;

VortexTasks_Module VortexTasks_NewModule(VortexTasks_Module_NewDataFunc_Type module_newdata_func,
     VortexTasks_ModuleDestructor_Type module_destructor) {
    VortexTasks_Module module = vortex_new_mem_of_type(struct VortexTasks_Module_s);
    module->data = module_newdata_func();
    module->destructor = module_destructor;
    mtx_init(&(module->mutex), mtx_plain);
    module->refcount = 0;
    module->tid = -2;
    module->dead = 0;
    return module;
}

void VortexTasks_AddToModuleRefCount(VortexTasks_Module module) {
    module->refcount++;
}

void VortexTasks_DestroyModule(VortexTasks_Module module, vortex_int tid) {
    VortexTasks_LockModule(module);
    if ( module->tid == -2 ) module->tid = tid;
    VortexTasks_UnLockModule(module);
    if ( module->tid != tid ) return;
    module->destructor(module);
    module->dead = 1;
    while ( VortexTasks_IsModuleLocked(module) ) {}
    mtx_destroy(&(module->mutex));
    free(module->data);
    free(module);
}

void VortexTasks_DeleteModule(VortexTasks_Module module, vortex_int tid) {
    module->refcount--;
    if ( module->refcount <= 0 ) VortexTasks_DestroyModule(module,tid);
}

vortex_int VortexTasks_LockModule(VortexTasks_Module module) {
    if ( module->dead ) return -1;
    if ( mtx_lock(&(module->mutex)) == thrd_success ) return 0;
    return 1;
}

vortex_int VortexTasks_IsModuleLocked(VortexTasks_Module module) {
    if ( mtx_trylock(&(module->mutex)) ) {
        mtx_unlock(&(module->mutex));
        return 0;
    }
    return 1;
}

vortex_int VortexTasks_UnLockModule(VortexTasks_Module module) {
    return mtx_unlock(&(module->mutex));
}

VortexAny VortexTasks_GetDataFromModule(VortexTasks_Module module) {
    return module->data;
}

static vortex_int Vortex_GetNumberOfThreadsForPlatform(vortex_int max_num_of_threads) {
    #ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int num_of_threads = sysinfo.dwNumberOfProcessors;
    #else
    vortex_int num_of_threads = (int) sysconf( _SC_NPROCESSORS_ONLN );
    #endif
    if ( num_of_threads > max_num_of_threads ) num_of_threads = max_num_of_threads;
    return num_of_threads;
}

VortexTasks_TaskGroup VortexTasks_NewTaskGroup(void) {
    VortexTasks_TaskGroup taskgroup = 
      vortex_new_mem_of_type(struct VortexTasks_TaskGroup_s);
    taskgroup->active = 1;
    taskgroup->dead = 0;
    taskgroup->done = 0;
    taskgroup->bound = 0;
    taskgroup->run_id = 0;
    taskgroup->num_of_tasks = 0;
    taskgroup->num_of_done_tasks = 0;
    taskgroup->num_of_dead_tasks = 0;
    taskgroup->task_array = NULL;
    mtx_init(&taskgroup->done_tasks_mutex, mtx_plain);
    cnd_init(&taskgroup->done_tasks_cond);
    return taskgroup;
}

static void VortexTasks_DestroyTask(VortexTasks_Task task, vortex_int tid) {
    free(task->this_task);
    VortexTasks_DeleteModule(task->module, tid);
    free(task);
}

void VortexTasks_DestroyTaskGroup(VortexTasks_TaskGroup taskgroup, vortex_int tid) {
    vortex_int i = 0;
    while (i < taskgroup->num_of_tasks) {
        VortexTasks_DestroyTask(taskgroup->task_array[i], tid);
        i++ ;
    }
    mtx_destroy(&taskgroup->done_tasks_mutex);
    cnd_destroy(&taskgroup->done_tasks_cond);
    free(taskgroup->task_array);
    free(taskgroup);
}

void VortexTasks_SetTaskGroupActive(VortexTasks_TaskGroup taskgroup, vortex_int active) {
    if ( active ) taskgroup->active = 1;
    if ( !active ) taskgroup->active = 0;
}

vortex_int VortexTasks_IsTaskGroupActive(VortexTasks_TaskGroup taskgroup) {
    return taskgroup->active;
}

vortex_int VortexTasks_IsTaskGroupDone(VortexTasks_TaskGroup taskgroup) {
    return taskgroup->done;
}

vortex_int VortexTasks_IsTaskGroupDead(VortexTasks_TaskGroup taskgroup) {
    return taskgroup->dead;
}

void VortexTasks_ResetTaskGroup(VortexTasks_TaskGroup taskgroup) {
    mtx_lock(&taskgroup->done_tasks_mutex);
    if ( !taskgroup->done ) {
        mtx_unlock(&taskgroup->done_tasks_mutex);
        return;
    }
    taskgroup->num_of_done_tasks = 0;
    taskgroup->run_id = !taskgroup->run_id;
    taskgroup->done = 0;
    mtx_unlock(&taskgroup->done_tasks_mutex);
}

void VortexTasks_BindTaskGroupToThreadGroup(VortexTasks_TaskGroup taskgroup,
       VortexTasks_ThreadGroup threadgroup ) {
    if ( !threadgroup->init ) return;
    VortexTasks_SleepThreadGroup(threadgroup);
    taskgroup->bound = 1;
    threadgroup->taskgroup = taskgroup;
    VortexTasks_AwakeThreadGroup(threadgroup);
}

void VortexTasks_UnbindTaskGroupFromThreadGroup(VortexTasks_TaskGroup taskgroup,
      VortexTasks_ThreadGroup threadgroup) {
    if ( !threadgroup->init || !taskgroup->bound ) return;
    VortexTasks_SleepThreadGroup(threadgroup);
    taskgroup->bound = 0;
    threadgroup->taskgroup = NULL;
    VortexTasks_AwakeThreadGroup(threadgroup);
}

void VortexTasks_AddTasks_Func(VortexTasks_TaskGroup taskgroup, vortex_ulong num_of_tasks,
     vortex_int (*task_func)(VortexTasks_Module, struct VortexTasks_ThisTask_s *), VortexTasks_Module module) {
    vortex_int i = 0;
    VortexTasks_Task task = NULL;
    VortexTasks_ThisTask thistask = NULL;
    if ( taskgroup->bound ) return;
    taskgroup->num_of_tasks += num_of_tasks;
    if ( taskgroup->task_array == NULL ) {
        taskgroup->task_array = vortex_c_array(taskgroup->num_of_tasks, VortexTasks_Task);
    } else {
        taskgroup->task_array = vortex_realloc(taskgroup->task_array, taskgroup->num_of_tasks, taskgroup->num_of_tasks-num_of_tasks, VortexTasks_Task, 1);
    }
    i = taskgroup->num_of_tasks-num_of_tasks;
    while ( i < taskgroup->num_of_tasks ) {
        thistask = vortex_new_mem_of_type(struct VortexTasks_ThisTask_s);
        thistask->active = 1;
        thistask->delete = 0;
        thistask->thread_id = -3;
        thistask->task_id = i;
        task = vortex_new_mem_of_type(struct VortexTasks_Task_s);
        task->done = 0;
        task->run_id = 0;
        task->this_task = thistask;
        task->task_func = task_func;
        VortexTasks_AddToModuleRefCount(module);
        task->module = module;
        taskgroup->task_array[i] = task;
        i++;
    }

}

static void VortexTasks_DeadDoneCheck(vortex_int* dead, int* done, vortex_int num_of_tasks,
     VortexTasks_TaskGroup taskgroup, VortexTasks_ThreadGroup threadgroup, vortex_int tid ) {
    mtx_lock(&taskgroup->done_tasks_mutex);
    taskgroup->num_of_dead_tasks += threadgroup->threadarray[tid].num_of_dead_tasks;
    taskgroup->num_of_done_tasks += threadgroup->threadarray[tid].num_of_done_tasks;
    threadgroup->threadarray[tid].num_of_dead_tasks = 0;
    threadgroup->threadarray[tid].num_of_done_tasks = 0;
    if ( taskgroup->num_of_dead_tasks == num_of_tasks ) *dead = 1;
    if ( taskgroup->num_of_done_tasks == num_of_tasks ) *done = 1;
    mtx_unlock(&taskgroup->done_tasks_mutex);
}

static vortex_int VortexTasks_WorkerThread(VortexAny argument) {
    VortexTasks_ThreadArgs threadargs = argument;
    VortexTasks_ThreadGroup threadgroup = threadargs->threadgroup;
    VortexTasks_TaskGroup taskgroup = NULL;
    VortexTasks_Task task = NULL;
    vortex_int tid = threadargs->tid;
    vortex_int index = threadargs->tid;
    while ( threadgroup->alive && threadgroup->threadarray[tid].alive ) {
        while ( threadgroup->awake && threadgroup->threadarray[tid].awake ) {
            if ( !threadgroup->alive || !threadgroup->threadarray[tid].alive ) break;
            threadgroup->threadarray[tid].sleep = 0;
            if ( (taskgroup != threadgroup->taskgroup) && (threadgroup->taskgroup != NULL) ) 
              taskgroup = threadgroup->taskgroup;
            if ( (taskgroup == NULL) || (taskgroup->task_array == NULL) ) continue;
            if ( (taskgroup->active) && (!taskgroup->done) && (!taskgroup->dead)) {
              task = (index >= taskgroup->num_of_tasks) ? NULL : taskgroup->task_array[index];
              if ( task != NULL ) {
                if ( (task->this_task->active) && (task->run_id == taskgroup->run_id) ) {
                    task->this_task->thread_id = tid;
                    task->done = task->task_func(task->module,task->this_task);
                    if ( task->done ) {
                        task->run_id = !task->run_id;
                        threadgroup->threadarray[tid].num_of_done_tasks++;
                    }
                    if ( task->this_task->delete ) {
                        VortexTasks_DestroyTask(task,tid);
                        taskgroup->task_array[index] = NULL;
                        threadgroup->threadarray[tid].num_of_dead_tasks++;
                    }
                }
              }
              index += threadgroup->num_of_threads;
            }
            if ( index >= taskgroup->num_of_tasks ) {
                index = tid;
                VortexTasks_DeadDoneCheck(&taskgroup->dead, &taskgroup->done,
                     taskgroup->num_of_tasks, taskgroup, threadgroup, tid);
                if ( (taskgroup->done) || (taskgroup->dead) ) cnd_signal(&taskgroup->done_tasks_cond);
            }
        }
        threadgroup->threadarray[tid].sleep = 1;
    }
    threadgroup->threadarray[tid].dead = 1;
    free(threadargs);
    return 0;
}

static vortex_int Vortex_SpawnThreads(VortexTasks_ThreadGroup threadgroup) {
    VortexTasks_ThreadArgs threadargs = NULL;
    vortex_int error = 0;
    vortex_int t = 0;
    threadgroup->threadarray = vortex_c_array(threadgroup->max_num_of_threads, VortexThread);
    while ( t < threadgroup->max_num_of_threads ) {
        threadargs = vortex_new_mem_of_type(struct VortexTasks_ThreadArgs_s);
        threadargs->threadgroup = threadgroup;
        threadargs->tid = t;
        threadgroup->threadarray[t].alive = 1;
        threadgroup->threadarray[t].dead = 0;
        threadgroup->threadarray[t].awake = 1;
        threadgroup->threadarray[t].sleep = 0;
        threadgroup->threadarray[t].num_of_done_tasks = 0;
        threadgroup->threadarray[t].num_of_dead_tasks = 0;
        error = 
          thrd_create(&(threadgroup->threadarray[t].thread),
               VortexTasks_WorkerThread, (void *)threadargs);
        if ( error != thrd_success ) return error;
        error = thrd_detach( threadgroup->threadarray[t].thread );
        if ( error != thrd_success ) return error;
        t++;
    }
    threadgroup->num_of_threads = t;
    threadgroup->init = 1;
    return error;
}

vortex_int VortexTasks_StartThreadGroup(VortexTasks_ThreadGroup threadgroup) {
    vortex_int error = 0;
    if ( !threadgroup->init ) {
        error = Vortex_SpawnThreads(threadgroup);
        if ( error != thrd_success ) return -1;
    }
    return 0;
}

VortexTasks_ThreadGroup VortexTasks_NewThreadGroup(vortex_int max_num_of_threads) {
    VortexTasks_ThreadGroup threadgroup = vortex_new_mem_of_type(struct VortexTasks_ThreadGroup_s);
    threadgroup->threadarray = NULL;
    threadgroup->max_num_of_threads = Vortex_GetNumberOfThreadsForPlatform(max_num_of_threads) ;
    threadgroup->num_of_threads = 0;
    threadgroup->taskgroup = NULL;
    threadgroup->awake = 0;
    threadgroup->alive = 1;
    threadgroup->init = 0;
    return threadgroup;
}

void VortexTasks_DestroyThreadGroup(VortexTasks_ThreadGroup threadgroup) {
    VortexTasks_KillAllThreads(threadgroup);
    free(threadgroup->threadarray);
    threadgroup->threadarray = NULL;
    free(threadgroup);
}

void VortexTasks_WaitForTasksToBeDone(VortexTasks_TaskGroup taskgroup) {
    if ( taskgroup->num_of_tasks == 0 ) return;
    if ( (taskgroup->done) || (taskgroup->dead) ) return;
    mtx_lock(&taskgroup->done_tasks_mutex);
    cnd_wait(&taskgroup->done_tasks_cond, &taskgroup->done_tasks_mutex);
    mtx_unlock(&taskgroup->done_tasks_mutex);
    while (!taskgroup->done) {}
}

vortex_int VortexTasks_AllThreadsDead(VortexTasks_ThreadGroup threadgroup) {
    vortex_int i = 0;
    while ( i < threadgroup->num_of_threads ) {
        if ( !threadgroup->threadarray[i].dead ) return 0;
        i++;
    }
    return 1;
}

void VortexTasks_KillAllThreads(VortexTasks_ThreadGroup threadGroup) {
    threadGroup->alive = 0;
    while ( 1 ) {
        if ( VortexTasks_AllThreadsDead(threadGroup) ) break ;
    }
}

vortex_int VortexTasks_KillThreadWithTid(VortexTasks_ThreadGroup threadgroup, vortex_int tid) {
    if ( (tid < 0) || (tid > threadgroup->num_of_threads) ) return -1;
    threadgroup->threadarray[tid].alive = 0;
    while ( 1 ) {
        if ( threadgroup->threadarray[tid].dead == 1 ) break;
    }
    return 0;
}

vortex_int VortexTasks_AllThreadsSleep(VortexTasks_ThreadGroup threadgroup) {
    vortex_int i = 0;
    while ( i < threadgroup->num_of_threads ) {
        if ( !threadgroup->threadarray[i].sleep ) return 0;
        i++;
    }
    return 1;
}

void VortexTasks_SleepThreadGroup(VortexTasks_ThreadGroup threadgroup) {
    threadgroup->awake = 0;
    while ( 1 ) {
        if ( VortexTasks_AllThreadsSleep(threadgroup) ) break;
    }
}

vortex_int VortexTasks_SleepThreadWithTid(VortexTasks_ThreadGroup threadgroup, vortex_int tid ) {
    if ( (tid < 0) || (tid > threadgroup->num_of_threads) ) return -1;
    threadgroup->threadarray[tid].awake = 0;
    while ( 1 ) {
        if ( threadgroup->threadarray[tid].sleep == 1 ) break;
    }
    return 0;
}

vortex_int VortexTasks_AllThreadsAwake(VortexTasks_ThreadGroup threadgroup) {
    vortex_int i = 0;
    while ( i < threadgroup->num_of_threads ) {
        if ( threadgroup->threadarray[i].sleep ) return 0;
        i++;
    }
    return 1;
}

void VortexTasks_AwakeThreadGroup(VortexTasks_ThreadGroup threadgroup) {
    threadgroup->awake = 1;
}

vortex_int VortexTasks_IsThreadGroupAwake(VortexTasks_ThreadGroup threadgroup) {
    return threadgroup->awake;
}

vortex_int VortexTasks_GetNumOfThreads(VortexTasks_ThreadGroup threadgroup) {
    return threadgroup->num_of_threads;
}

vortex_int VortexTasks_AwakeThreadWithTid(VortexTasks_ThreadGroup threadgroup, vortex_int tid) {
    if ( (tid < 0) || (tid > threadgroup->num_of_threads) ) return -1;
    threadgroup->threadarray[tid].awake = 1;
    while ( 1 ) {
        if ( threadgroup->threadarray[tid].sleep == 0 ) break;
    }
    return 0;
}

void VortexTasks_DeactivateTask(VortexTasks_ThisTask thistask) {
    thistask->active = 0;
}

void VortexTasks_DeleteTask(VortexTasks_ThisTask thistask) {
    thistask->delete = 1;
}

vortex_int VortexTasks_GetTaskID(VortexTasks_ThisTask thistask) {
    return thistask->task_id;
}

vortex_int VortexTasks_GetThreadID(VortexTasks_ThisTask thistask) {
    return thistask->thread_id;
}
