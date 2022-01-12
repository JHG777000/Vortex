/*
 Copyright (c) 2014-2019 Jacob Gordon. All rights reserved.

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
#include <RKLib/RKMem.h>
#include <RKLib/RKArgs.h>
#include <RKLib/RKMath.h>
#include <RKLib/RKTasks.h>

RKTasks_DefineModule(TestMod, int value ; int value2 ; int counter ; RKMath_NewVector(Vec,3); RKMath_NewVector(Vec2,3);) {

    RKTasks_GetModuleData(TestMod,module)->value = 0 ;

    printf("From Mod Destructor: %d!!!!!\n",RKTasks_GetModuleData(TestMod,module)->value2) ;
}

RKTasks_CreateTask(TestTask) { //Create a Task called TestTask

    int counter = 0 ;

    RKTasks_LockModule(module) ;

    RKTasks_GetModuleData(TestMod,module)->counter++ ;

    counter = RKTasks_GetModuleData(TestMod,module)->counter ;

    RKTasks_UnLockModule(module) ;

    int value = RKTasks_GetModuleData(TestMod,module)->value ;

    if ( value == 5 ) {

        RKMath_NewVector(out, 3) ;

        RKMath_NewVector(Vec,3) ;

        RKMath_NewVector(Vec2,3) ;

        RKMath_Equal(Vec, RKTasks_GetModuleData(TestMod,module)->Vec, 3) ;

        RKMath_Equal(Vec2, RKTasks_GetModuleData(TestMod,module)->Vec2, 3) ;

        RKMath_Mul(out, Vec, Vec2, 3) ;

        printf("Task Id: %d: VecOut:%f %f %f %f\n", RKTasks_GetTaskID(thistask), out[RKM_X], out[RKM_Y], out[RKM_Z], RKMath_Dot(out, Vec2, 3)) ;

    } else {

        printf("NOT 5, NOOOOO!!!!!\n") ;
    }

    RKMath_RandState randstate ;

    RKMath_SeedRandomState(&randstate, RKTasks_GetTaskID(thistask) + counter) ;

    printf("%d, a random number from task: %d, with count: %d\n", RKMath_ARandomNumber(&randstate, 0, 5000), RKTasks_GetTaskID(thistask), counter) ;

    RKTasks_LockModule(module) ;

    RKTasks_GetModuleData(TestMod,module)->value2 = counter ;

    RKTasks_UnLockModule(module) ;

    return 1 ; //return 0 or false, if not done
}


void rkargs_example3( RKArgs args ) {

    printf("And once more:\n") ;

    RKArgs_UseArgs(args) ;

    printf("%d\n",RKArgs_GetNextArg(args, int)) ;

    printf("%d\n",RKArgs_GetNextArg(args, int)) ;

    printf("%d\n",RKArgs_GetNextArg(args, int)) ;

    printf("%s\n",RKArgs_GetNextArg(args, char*)) ;

}

RKArgs rkargs_example2( RKArgs args, RKArgs args2 ) {

    RKArgs_UseArgs(args) ;

    if ( RKArgs_CanGetNextArg(args, int) ) printf("Can get arg!!!!\n") ;

    printf("%d\n",RKArgs_GetNextArg(args, int)) ;

    printf("%d\n",RKArgs_GetNextArg(args, int)) ;

    printf("%d\n",RKArgs_GetNextArg(args, int)) ;

    printf("%s\n",RKArgs_GetNextArg(args, char*)) ;

    printf("%s\n",RKArgs_GetNextArg(args2, char*)) ;

    printf("%s\n",RKArgs_GetNextArg(args2, char*)) ;

    printf("%20.18f\n",RKArgs_GetNextArg(args2, float)) ;

    printf("%20.18f\n",RKArgs_GetNextArg(args2, double)) ;

    printf("%hd\n",RKArgs_GetNextArg(args2, short)) ;

    if ( !RKArgs_CanGetNextArg(args2, short) ) printf("Can not get arg!!!!\n") ;

    return RKArgs_CloneArgs(args) ;
}

RKArgs rkargs_example1(void) {

    return rkargs_example2( newargs(ints(347342,62,754),args(char*,"Hello")), newargs(args(char*,"World!!!!","and stuff"),

                                                                     args(float,3.141592653589793238),args(double,3.141592653589793238),shorts(3)) ) ;
}

int main(int argc, const char * argv[]) {

    RKArray array = RKArray_NewArray() ;

    RKArray_AddItem(array,rkstr("Hello World!!! 1\n")) ;

    RKArray_AddItem(array,rkstr("Hello World!!! 2\n")) ;

    RKArray_AddItem(array,rkstr("Hello World!!! 3\n")) ;

    rkstrprint(RKArray_GetItem(array,0)) ;

    rkstrprint(RKArray_GetItem(array,1)) ;

    rkstrprint(RKArray_GetItem(array,2)) ;

    RKString_DestroyString(RKArray_GetItem(array,2)) ;

    RKArray_SetItem(array,2,rkstr("Hello World!!! 4\n")) ;

    rkstrprint(RKArray_GetItem(array,2)) ;

    RKArray_DestroyArray(array) ;

    RKULong numbers[] = {1,2,3,4,5} ;

    RKArray Numbers = RKArray_NewArray() ;

    RKArray_AddArray(Numbers,numbers,sizeof(numbers)) ;

    printf("%d\n",RKArray_GetItem(Numbers,0)) ;

    RKArray_DestroyArray(Numbers) ;

    RKArray Numbers2 = RKMem_NewArray(numbers) ;

    RKArray_AddArray(Numbers2,numbers,sizeof(numbers)) ;

    printf("%d\n",RKArray_GetItem(Numbers2,0)) ;

    RKArray_DestroyArray(Numbers2) ;

    RKArgs args = rkargs_example1() ;

    rkargs_example3(args) ;

    RKArgs_DestroyClonedArgs(args) ;

    int offset = 0 ;

    RKString string = rkstr("Hello Earth!!!!\n"
                            "Hi!\n") ;
    rkstrprint(string) ;

    RKString string2 = rkstrU8( "12345678\n"
                               "Hi😀!\n"
                               "😀, and 😀?!!!!\n") ;

    RKString string3 = RKString_NewStringFromUTF32(L"😀and,😀\n",7) ;

    if ( RKString_GetCharacter(string2, 11, &offset) == L'😀' ) {

        printf("Hello:😀, and %lu\n",RKString_GetLength(string2)) ;

        printf("Hello2:😀, and %lu\n",RKString_GetLength(string3)) ;

        rkstrprint(string3) ;
    }

    rkstrprint(string2) ;

    rkstrprint(RKString_GetStringForASCII(string2)) ;

    RKAny any1 = rkany(string) ;

    rkstrprint(rkget(RKString,any1)) ;

    if ( RKMath_Sqrt(2.f) == 1.41386008f ) {

      puts("yes!!!") ;

    }

    int z = 0 ;

    z = 37482 ;

    RKAny any2 = rkany(z) ;

    printf("hello:%d\n",rkget(int, any2)) ;

    rkstrfree(string) ;

    rkstrfree(string2) ;

    free(any1) ;

    free(any2) ;

    //Note: Due to RKTasks flexible design, some of the following lines of code could be rearranged

    RKTasks_ThreadGroup ThreadGroup = RKTasks_NewThreadGroup(10) ;

    RKTasks_TaskGroup TaskGroup = RKTasks_NewTaskGroup() ;

    RKTasks_StartThreadGroup(ThreadGroup) ;

    RKTasks_Module module = RKTasks_CreateModule(TestMod) ;

    RKTasks_GetModuleData(TestMod,module)->value = 5 ;

    RKTasks_GetModuleData(TestMod,module)->value2 = 0 ;

    RKTasks_GetModuleData(TestMod,module)->counter = 0 ;

    RKMath_NewVector(MyVec, 3) ; //Creates a 3-dimensional vector called MyVec

    RKMath_Vectorit(MyVec2, 1, 2, 3) ; //Creates a 3-dimensional vector called MyVec2, with the values 1,2 and 3

    RKMath_Set_Vec_Equal_To_A_Const(MyVec, 3, 3) ; //Sets all MyVec components to equal to 3

    MyVec2[RKM_X]++ ; //Add one to the X-value of MyVec2

    RKMath_Equal(RKTasks_GetModuleData(TestMod,module)->Vec, MyVec, 3) ;

    RKMath_Equal(RKTasks_GetModuleData(TestMod,module)->Vec2, MyVec2, 3) ;

    RKTasks_AddTasks(TaskGroup, 2000, TestTask, module) ; //can not be called on a bound taskgroup

    RKTasks_BindTaskGroupToThreadGroup(TaskGroup, ThreadGroup) ;

    RKTasks_WaitForTasksToBeDone(TaskGroup) ;

    RKTasks_ResetTaskGroup(TaskGroup) ;

    RKTasks_WaitForTasksToBeDone(TaskGroup) ;

    RKTasks_ResetTaskGroup(TaskGroup) ;

    RKTasks_WaitForTasksToBeDone(TaskGroup) ;

    RKTasks_DestroyThreadGroup(ThreadGroup) ;

    RKTasks_DestroyTaskGroup(TaskGroup, RKTasks_MainThreadID) ;

    printf("Hello World!!!!!\n") ;

    return 0 ;
}
