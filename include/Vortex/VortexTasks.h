/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexTasks.h
//Header file for VortexTasks.

#ifndef Vortex_VortexTasks_h
#define VortexLib_VortexTasks_h

#include <Vortex/VortexMem.h>

typedef struct VortexTasks_Module_s* VortexTasks_Module;
typedef void* (*VortexTasks_Module_NewDataFunc_Type)(void);
typedef void (*VortexTasks_ModuleDestructor_Type)(VortexTasks_Module);
typedef struct VortexTasks_Task_s* VortexTasks_Task;
typedef struct VortexTasks_ThisTask_s* VortexTasks_ThisTask;
typedef struct VortexTasks_ThreadGroup_s* VortexTasks_ThreadGroup;
typedef struct VortexTasks_TaskGroup_s* VortexTasks_TaskGroup;
#define VortexTasks_MainThreadID -1
#define VortexTasks_DefineModule( modname, ... ) typedef struct struct_##modname##_s { __VA_ARGS__ }* modname;\
static void* modname##_##VortexTasks_Module_NewDataFunc_CreateModule(void) {\
return vortex_new_mem_of_type(struct struct_##modname##_s);}\
static void modname##_##VortexTasks_ModuleDestructor(VortexTasks_Module module)
#define VortexTasks_CreateModule( modname ) VortexTasks_NewModule( modname##_##VortexTasks_Module_NewDataFunc_CreateModule, modname##_##VortexTasks_ModuleDestructor )
#define VortexTasks_GetModuleData( modname, module ) ((modname)VortexTasks_GetDataFromModule(module))
#define VortexTasks_CreateTask(TaskName) static int TaskName##_TaskFunc(VortexTasks_Module module, struct VortexTasks_ThisTask_s * thistask)
#define VortexTasks_AddTasks( taskgroup, num_of_tasks, taskfunc, module ) VortexTasks_AddTasks_Func(taskgroup,num_of_tasks,taskfunc##_TaskFunc,module)

VortexTasks_Module VortexTasks_NewModule(VortexTasks_Module_NewDataFunc_Type module_newdata_func, VortexTasks_ModuleDestructor_Type module_destructor);
void VortexTasks_AddToModuleRefCount(VortexTasks_Module module);
void VortexTasks_DestroyModule(VortexTasks_Module module, vortex_int tid);
void VortexTasks_DeleteModule(VortexTasks_Module module, vortex_int tid);
vortex_int VortexTasks_LockModule(VortexTasks_Module module);
vortex_int VortexTasks_IsModuleLocked(VortexTasks_Module module);
vortex_int VortexTasks_UnLockModule(VortexTasks_Module module);
VortexAny VortexTasks_GetDataFromModule(VortexTasks_Module module);
VortexTasks_TaskGroup VortexTasks_NewTaskGroup(void);
void VortexTasks_DestroyTaskGroup(VortexTasks_TaskGroup taskgroup, vortex_int tid);
void VortexTasks_SetTaskGroupActive(VortexTasks_TaskGroup taskgroup, vortex_int active);
vortex_int VortexTasks_IsTaskGroupActive(VortexTasks_TaskGroup taskgroup);
vortex_int VortexTasks_IsTaskGroupDone(VortexTasks_TaskGroup taskgroup);
vortex_int VortexTasks_IsTaskGroupDead(VortexTasks_TaskGroup taskgroup );
void VortexTasks_ResetTaskGroup(VortexTasks_TaskGroup taskgroup);
void VortexTasks_BindTaskGroupToThreadGroup(VortexTasks_TaskGroup taskgroup, VortexTasks_ThreadGroup threadgroup );
void VortexTasks_AddTasks_Func(VortexTasks_TaskGroup taskgroup, vortex_ulong num_of_tasks, vortex_int (*task_func)(VortexTasks_Module, struct VortexTasks_ThisTask_s*), VortexTasks_Module module);
VortexTasks_ThreadGroup VortexTasks_NewThreadGroup( vortex_int max_num_of_threads);
void VortexTasks_DestroyThreadGroup(VortexTasks_ThreadGroup threadgroup);
vortex_int VortexTasks_StartThreadGroup(VortexTasks_ThreadGroup threadgroup);
void VortexTasks_WaitForTasksToBeDone(VortexTasks_TaskGroup taskgroup);
vortex_int VortexTasks_AllThreadsDead(VortexTasks_ThreadGroup threadgroup);
void VortexTasks_KillAllThreads(VortexTasks_ThreadGroup threadgroup);
vortex_int VortexTasks_KillThreadWithTid(VortexTasks_ThreadGroup threadgroup, vortex_int tid);
vortex_int VortexTasks_AllThreadsSleep(VortexTasks_ThreadGroup threadgroup);
void VortexTasks_SleepThreadGroup(VortexTasks_ThreadGroup threadgroup );
vortex_int VortexTasks_SleepThreadWithTid(VortexTasks_ThreadGroup threadgroup, vortex_int tid );
vortex_int VortexTasks_AllThreadsAwake(VortexTasks_ThreadGroup threadgroup);
void VortexTasks_AwakeThreadGroup(VortexTasks_ThreadGroup threadgroup);
vortex_int VortexTasks_IsThreadGroupAwake(VortexTasks_ThreadGroup threadgroup);
vortex_int VortexTasks_GetNumOfThreads(VortexTasks_ThreadGroup threadgroup);
vortex_int VortexTasks_AwakeThreadWithTid(VortexTasks_ThreadGroup threadgroup, vortex_int tid);
void VortexTasks_DeactivateTask(VortexTasks_ThisTask thistask);
void VortexTasks_DeleteTask(VortexTasks_ThisTask thistask);
vortex_int VortexTasks_GetTaskID(VortexTasks_ThisTask thistask);
vortex_int VortexTasks_GetThreadID( VortexTasks_ThisTask thistask );

#endif /* Vortex_VortexTasks_h */
