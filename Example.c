/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: Example.c
//Basic Example on how to use Vortex, especially VortexMath, and VortexTasks.

#include <stdio.h>
#include <stdlib.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexArgs.h>
#include <Vortex/VortexMath.h>
#include <Vortex/VortexTasks.h>

VortexTasks_DefineModule(TestMod, int value; int value2; int counter; 
      VortexMath_NewVector(Vec,3); VortexMath_NewVector(Vec2,3);) {
    VortexTasks_GetModuleData(TestMod,module)->value = 0;
    printf("From Mod Destructor: %d!!!!!\n",VortexTasks_GetModuleData(TestMod,module)->value2);
}

VortexTasks_CreateTask(TestTask) { //Create a Task called TestTask
    int counter = 0;
    VortexTasks_LockModule(module);
    VortexTasks_GetModuleData(TestMod,module)->counter++;
    counter = VortexTasks_GetModuleData(TestMod,module)->counter;
    VortexTasks_UnLockModule(module);
    int value = VortexTasks_GetModuleData(TestMod,module)->value;
    if ( value == 5 ) {
        VortexMath_NewVector(out, 3);
        VortexMath_NewVector(Vec,3);
        VortexMath_NewVector(Vec2,3);
        VortexMath_Equal(Vec, VortexTasks_GetModuleData(TestMod,module)->Vec, 3);
        VortexMath_Equal(Vec2, VortexTasks_GetModuleData(TestMod,module)->Vec2, 3);
        VortexMath_Mul(out, Vec, Vec2, 3);
        printf("Task Id: %d: VecOut:%f %f %f %f\n", VortexTasks_GetTaskID(thistask), 
            out[VORTEX_X], out[VORTEX_Y], out[VORTEX_Z], VortexMath_Dot(out, Vec2, 3));
    } else {
        printf("NOT 5, NOOOOO!!!!!\n");
    }
    VortexMath_RandState randstate;
    VortexMath_SeedRandomState(&randstate, VortexTasks_GetTaskID(thistask) + counter);
    printf("%d, a random number from task: %d, with count: %d\n", 
        VortexMath_ARandomNumber(&randstate, 0, 5000), VortexTasks_GetTaskID(thistask), counter);
    VortexTasks_LockModule(module) ;
    VortexTasks_GetModuleData(TestMod,module)->value2 = counter;
    VortexTasks_UnLockModule(module);
    return 1; //return 0 or false, if not done
}

void vortex_args_example3(VortexArgs args) {
    printf("And once more:\n");
    VortexArgs_UseArgs(args);
    printf("%d\n",VortexArgs_GetNextArg(args, int));
    printf("%d\n",VortexArgs_GetNextArg(args, int));
    printf("%d\n",VortexArgs_GetNextArg(args, int));
    printf("%s\n",VortexArgs_GetNextArg(args, char*));
}

VortexArgs vortex_args_example2(VortexArgs args, VortexArgs args2) {
    VortexArgs_UseArgs(args);
    if ( VortexArgs_CanGetNextArg(args, int) ) printf("Can get arg!!!!\n");
    printf("%d\n",VortexArgs_GetNextArg(args, int));
    printf("%d\n",VortexArgs_GetNextArg(args, int));
    printf("%d\n",VortexArgs_GetNextArg(args, int));
    printf("%s\n",VortexArgs_GetNextArg(args, char*));
    printf("%s\n",VortexArgs_GetNextArg(args2, char*));
    printf("%s\n",VortexArgs_GetNextArg(args2, char*));
    printf("%20.18f\n",VortexArgs_GetNextArg(args2, float));
    printf("%20.18f\n",VortexArgs_GetNextArg(args2, double));
    printf("%hd\n",VortexArgs_GetNextArg(args2, short));
    if ( !VortexArgs_CanGetNextArg(args2, short) ) printf("Can not get arg!!!!\n");
    return VortexArgs_CloneArgs(args);
}

VortexArgs vortex_args_example1(void) {
    return vortex_args_example2( newargs(ints(347342,62,754),args(char*,"Hello")), 
             newargs(args(char*,"World!!!!","and stuff"),args(float,3.141592653589793238),
               args(double,3.141592653589793238),shorts(3)) );
}

int main(int argc, const char *argv[]) {
    VortexArray array = VortexArray_New();
    VortexArray_AddItem(array,vortex_str("Hello World!!! 1\n"));
    VortexArray_AddItem(array,vortex_str("Hello World!!! 2\n"));
    VortexArray_AddItem(array,vortex_str("Hello World!!! 3\n"));
    vortex_strprint(VortexArray_GetItem(array,0));
    vortex_strprint(VortexArray_GetItem(array,1));
    vortex_strprint(VortexArray_GetItem(array,2));
    VortexString_Destroy(VortexArray_GetItem(array,2));
    VortexArray_SetItem(array,2,vortex_str("Hello World!!! 4\n"));
    vortex_strprint(VortexArray_GetItem(array,2));
    VortexArray_Destroy(array);
    vortex_ulong numbers[] = {1,2,3,4,5};
    VortexArray Numbers = VortexArray_New();
    VortexArray_AddArray(Numbers,numbers,sizeof(numbers));
    printf("%d\n",VortexArray_GetItem(Numbers,0));
    VortexArray_Destroy(Numbers);
    VortexArray Numbers2 = vortex_new_array(numbers);
    VortexArray_AddArray(Numbers2,numbers,sizeof(numbers));
    printf("%d\n",VortexArray_GetItem(Numbers2,0));
    VortexArray_Destroy(Numbers2);
    VortexArgs args = vortex_args_example1();
    vortex_args_example3(args);
    VortexArgs_DestroyClonedArgs(args);
    int offset = 0;
    VortexString string = vortex_str("Hello Earth!!!!\n"
                            "Hi!\n");
    vortex_strprint(string);
    VortexString string2 = vortex_strU8( "12345678\n"
                               "Hi😀!\n"
                               "😀, and 😀?!!!!\n");

    VortexString string3 = VortexString_NewFromUTF32(L"😀and,😀\n",7);

    if ( VortexString_GetCharacter(string2, 11, &offset) == L'😀' ) {
        printf("Hello:😀, and %lu\n",VortexString_GetLength(string2));
        printf("Hello2:😀, and %lu\n",VortexString_GetLength(string3));
        vortex_strprint(string3);
    }
    vortex_strprint(string2);
    vortex_strprint(VortexString_GetStringForASCII(string2));
    VortexAny any1 = vortex_any(string);

    vortex_strprint(vortex_get(VortexString,any1));

    if ( VortexMath_Sqrt(2.f) == 1.41386008f ) {
      puts("yes!!!");
    }
    int z = 0;
    z = 37482;
    VortexAny any2 = vortex_any(z);
    printf("hello:%d\n",vortex_get(int, any2));
    vortex_strfree(string);
    vortex_strfree(string2);
    free(any1);
    free(any2);
    //Note: Due to VortexTasks flexible design, some of the following lines of code could be rearranged
    VortexTasks_ThreadGroup ThreadGroup = VortexTasks_NewThreadGroup(10);
    VortexTasks_TaskGroup TaskGroup = VortexTasks_NewTaskGroup();
    VortexTasks_StartThreadGroup(ThreadGroup);
    VortexTasks_Module module = VortexTasks_CreateModule(TestMod);
    VortexTasks_GetModuleData(TestMod,module)->value = 5;
    VortexTasks_GetModuleData(TestMod,module)->value2 = 0;
    VortexTasks_GetModuleData(TestMod,module)->counter = 0;
    VortexMath_NewVector(MyVec, 3); //Creates a 3-dimensional vector called MyVec
    VortexMath_Vectorit(MyVec2, 1, 2, 3); //Creates a 3-dimensional vector called MyVec2, with the values 1,2 and 3
    VortexMath_Set_Vec_Equal_To_A_Const(MyVec, 3, 3); //Sets all MyVec components to equal to 3
    MyVec2[VORTEX_X]++; //Add one to the X-value of MyVec2
    VortexMath_Equal(VortexTasks_GetModuleData(TestMod,module)->Vec, MyVec, 3);
    VortexMath_Equal(VortexTasks_GetModuleData(TestMod,module)->Vec2, MyVec2, 3);
    VortexTasks_AddTasks(TaskGroup, 2000, TestTask, module); //can not be called on a bound taskgroup
    VortexTasks_BindTaskGroupToThreadGroup(TaskGroup, ThreadGroup);
    VortexTasks_WaitForTasksToBeDone(TaskGroup);
    VortexTasks_ResetTaskGroup(TaskGroup);
    VortexTasks_WaitForTasksToBeDone(TaskGroup);
    VortexTasks_ResetTaskGroup(TaskGroup);
    VortexTasks_WaitForTasksToBeDone(TaskGroup);
    VortexTasks_DestroyThreadGroup(ThreadGroup);
    VortexTasks_DestroyTaskGroup(TaskGroup, VortexTasks_MainThreadID);
    VortexString format_output = VortexString_NewEmptyString(36);
    VortexString_FormatToString(format_output,"%s, and %f, and %d!\n","Hello World",1.2345f,34);
    VortexString_Print(format_output);
    VortexString_Destroy(format_output);
    void Vortex_LexerExample(void);
    Vortex_LexerExample();
    printf("Hello World!!!!!\n");
    return 0;
}
