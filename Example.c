/*
 Copyright (c) 2016 Jacob Gordon. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 
 following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//File: Example.c
//Basic Example on how to use RKLib. Specifically RKMath, and RKTasks.

#include <stdio.h>
#include <stdlib.h>
#include "RKMem.h"
#include "RKArgs.h"
#include "RKMath.h"
#include "RKTasks.h"

RKTasks_CreateTask(TestTask, int value ; RKMath_NewVector(Vec,3); RKMath_NewVector(Vec2,3);) { //Create a Task called TestTask
    
    static RKMAtomicInt counter = 0 ;
    
    int val = 0;
    
    RKMath_AtomicInc(&counter) ;
    
    val = counter ;
    
    if ( RKTArgs->value == 5 ) {
                     
        RKMath_NewVector(out, 3) ;
        
        RKMath_Mul(out, RKTArgs->Vec, RKTArgs->Vec2, 3) ;
        
       printf("Task Id: %d: VecOut:%f %f %f %f\n", RKTasks_GetTaskID(ThisTask), out[RKM_X], out[RKM_Y], out[RKM_Z], RKMath_Dot(out, RKTArgs->Vec2, 3)) ;
        
    } else {
        
       printf("NOT 5, NOOOOO!!!!!\n") ;
        
    }
    
    RKMath_RandState randstate ;
    
    RKMath_SeedRandomState(&randstate, RKTasks_GetTaskID(ThisTask) + val) ;
    
    printf("%d, a random number from task: %d, with count: %d\n", RKMath_ARandomNumber(&randstate, 0, 5000), RKTasks_GetTaskID(ThisTask), val) ;
}

void rkargs_example2( RKArgs args, RKArgs args2 ) {
    
    RKArgs_UseArgs(args) ;
    
    printf("%d\n",RKArgs_GetNextArg(args, int)) ;
    
    printf("%d\n",RKArgs_GetNextArg(args, int)) ;
    
    printf("%d\n",RKArgs_GetNextArg(args, int)) ;
    
    printf("%s\n",RKArgs_GetNextArg(args, char*)) ;
    
    printf("%s\n",RKArgs_GetNextArg(args2, char*)) ;
    
    printf("%s\n",RKArgs_GetNextArg(args2, char*)) ;
    
    printf("%20.18f\n",RKArgs_GetNextArg(args2, float)) ;
    
    printf("%20.18f\n",RKArgs_GetNextArg(args2, double)) ;
    
    printf("%hd\n",RKArgs_GetNextArg(args2, short)) ;
}

void rkargs_example1(void) {
    
    rkargs_example2( newargs(ints(347342,62,754),args(char*,"Hello")), newargs(args(char*,"World!!!!","and stuff"),
                                                                     
                                                                     args(float,3.141592653589793238),args(double,3.141592653589793238),shorts(3)) ) ;
}

int main(int argc, const char * argv[]) {
    
    rkargs_example1() ;
    
    //Note: Due to RKTasks flexible design, some of the following lines of code could be rearranged
    
    RKTasks_ThreadGroup ThreadGroup = RKTasks_NewThreadGroup(1, 10, 4, 1, 7) ;
    
    RKTasks_TaskGroup TaskGroup = RKTasks_NewTaskGroup() ;
    
    RKTasks_BindTaskGroupToThreadGroup(TaskGroup, ThreadGroup) ; //TaskGroups need to be bound or they will not run
    
    RKTasks_RunThreadGroup(ThreadGroup) ; //Activates a thread group, only an active thread group will run(this is also when thread creation happens)
    
    RKTasks_UseTaskGroup(TaskGroup) ; //Must be called before each time a TaskGroup is to run

    int i = 0 ;
    
    RKMath_NewVector(MyVec, 3) ; //Creates a 3-dimensional vector called MyVec
    
    RKMath_Vectorit(MyVec2, 1, 2, 3) ; //Creates a 3-dimensional vector called MyVec2, with the values 1,2 and 3
    
    RKMath_Set_Vec_Equal_To_A_Const(MyVec, 3, 3) ; //Sets all MyVec components to equal to 3
    
    MyVec2[RKM_X]++ ; //Add one to the X-value of MyVec2
    
    RKTasks_Args(TestTask) ; //Declares task args for TestTask
    
    while ( i < 2000 ) {
        
        RKTasks_UseArgs(TestTask) ; //Mallocs a new set of args for each task
        
        TestTask_Args->value = 5 ;
        
        RKMath_VectorCopy(TestTask_Args->Vec, MyVec) ; //Copy vector via macro
        
        RKMath_Equal(TestTask_Args->Vec2, MyVec2, 3) ; //Copy vector via function
        
        RKTasks_AddTask(TaskGroup, TestTask, TestTask_Args) ; //Add the task TestTask with args to the task group, TaskGroup
        
        i++ ;
    }
    
    RKTasks_WaitForTasksToBeDone(TaskGroup) ; //Wait for all task to run
    
    RKTasks_UseTaskGroup(TaskGroup) ; //Use the same TaskGroup for a second time
    
    RKTasks_WaitForTasksToBeDone(TaskGroup) ; //Wait for all task to run
    
    RKTasks_UseTaskGroup(TaskGroup) ;  //Use the same TaskGroup for a third time
    
    RKTasks_WaitForTasksToBeDone(TaskGroup) ; //Wait for all task to run
    
    RKTasks_KillThreadGroup(ThreadGroup) ; //Stop all threads and delete ThreadGroup
    
    RKTasks_KillTaskGroup(TaskGroup) ; //Delete TaskGroup
    
    printf("Hello World!!!!!\n") ;
    
    return 0 ;
}

