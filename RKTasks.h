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

//File: RKTasks.h
//Header file for RKTasks.

#ifndef RKTasks4_h
#define RKTasks4_h

#include "RKMem.h"

typedef struct RKTasks_Module_s* RKTasks_Module ;

typedef void* (*RKTasks_Module_NewDataFunc_Type)(void) ;

typedef void (*RKTasks_ModuleDestructor_Type)(RKTasks_Module) ;

typedef struct RKTasks_Task_s* RKTasks_Task ;

typedef struct RKTasks_ThisTask_s* RKTasks_ThisTask ;

typedef struct RKTasks_ThreadGroup_s* RKTasks_ThreadGroup ;

typedef struct RKTasks_TaskGroup_s* RKTasks_TaskGroup ;

#define RKTasks_MainThreadID -1

#define RKTasks_DefineModule( modname, destructor, ... ) typedef struct struct_##modname##_s { __VA_ARGS__ }* modname ;\
static void modname##_##RKTasks_ModuleDestructor( RKTasks_Module  module ){destructor;}\
static void* modname##_##RKTasks_Module_NewDataFunc_CreateModule( void ) {\
return RKMem_NewMemOfType( struct struct_##modname##_s ) ; }

#define RKTasks_CreateModule( modname ) RKTasks_NewModule( modname##_##RKTasks_Module_NewDataFunc_CreateModule, modname##_##RKTasks_ModuleDestructor )

#define RKTasks_GetModuleData( modname, module ) ((modname)RKTasks_GetDataFromModule(module))

#define RKTasks_CreateTask(TaskName) static int TaskName##_TaskFunc(RKTasks_Module module, struct RKTasks_ThisTask_s * thistask)

#define RKTasks_AddTasks( taskgroup, num_of_tasks, taskfunc, module ) RKTasks_AddTasks_Func(taskgroup,num_of_tasks,taskfunc##_TaskFunc,module)

RKTasks_Module RKTasks_NewModule( RKTasks_Module_NewDataFunc_Type module_newdatafunc, RKTasks_ModuleDestructor_Type moduledestructor) ;

void RKTAsks_AddToModuleRefCount( RKTasks_Module module ) ;

void RKTasks_DestroyModule( RKTasks_Module module, int tid ) ;

void RKTasks_DeleteModule( RKTasks_Module module, int tid ) ;

int RKTasks_LockModule( RKTasks_Module module ) ;

int RKTasks_IsModuleLocked( RKTasks_Module module ) ;

int RKTasks_UnLockModule( RKTasks_Module module ) ;

void* RKTasks_GetDataFromModule( RKTasks_Module module ) ;

RKTasks_TaskGroup RKTasks_NewTaskGroup( void ) ;

void RKTasks_DestroyTaskGroup( RKTasks_TaskGroup taskgroup, int tid ) ;

void RKTasks_SetTaskGroupActive( RKTasks_TaskGroup taskgroup, int active ) ;

int RKTasks_IsTaskGroupActive( RKTasks_TaskGroup taskgroup ) ;

int RKTasks_IsTaskGroupDone( RKTasks_TaskGroup taskgroup ) ;

int RKTasks_IsTaskGroupDead( RKTasks_TaskGroup taskgroup ) ;

void RKTasks_ResetTaskGroup( RKTasks_TaskGroup taskgroup ) ;

void RKTasks_BindTaskGroupToThreadGroup( RKTasks_TaskGroup taskgroup, RKTasks_ThreadGroup threadgroup ) ;

void RKTasks_AddTasks_Func( RKTasks_TaskGroup taskgroup, int num_of_tasks, int (*task_func)(RKTasks_Module, struct RKTasks_ThisTask_s *), RKTasks_Module module ) ;

RKTasks_ThreadGroup RKTasks_NewThreadGroup( int max_num_of_threads ) ;

void RKTasks_DestroyThreadGroup( RKTasks_ThreadGroup threadgroup ) ;

void RKTasks_StartThreadGroup( RKTasks_ThreadGroup threadgroup ) ;

void RKTasks_WaitForTasksToBeDone( RKTasks_TaskGroup taskgroup ) ;

int RKTasks_AllThreadsDead( RKTasks_ThreadGroup threadgroup ) ;

void RKTasks_KillAllThreads( RKTasks_ThreadGroup threadgroup ) ;

int RKTasks_KillThreadWithTid( RKTasks_ThreadGroup threadgroup, int tid ) ;

int RKTasks_AllThreadsSleep( RKTasks_ThreadGroup threadgroup ) ;

void RKTasks_SleepThreadGroup( RKTasks_ThreadGroup threadgroup ) ;

int RKTasks_SleepThreadWithTid( RKTasks_ThreadGroup threadgroup, int tid ) ;

int RKTasks_AllThreadsAwake( RKTasks_ThreadGroup threadgroup ) ;

void RKTasks_AwakeThreadGroup( RKTasks_ThreadGroup threadgroup ) ;

int RKTasks_IsThreadGroupAwake( RKTasks_ThreadGroup threadgroup ) ;

int RKTasks_GetNumOfThreads( RKTasks_ThreadGroup threadgroup ) ;

int RKTasks_AwakeThreadWithTid( RKTasks_ThreadGroup threadgroup, int tid ) ;

void RKTasks_DeactivateTask( RKTasks_ThisTask thistask ) ;

void RKTasks_DeleteTask( RKTasks_ThisTask thistask ) ;

int RKTasks_GetTaskID( RKTasks_ThisTask thistask ) ;

int RKTasks_GetThreadID( RKTasks_ThisTask thistask ) ;

#endif /* RKTasks4_h */
