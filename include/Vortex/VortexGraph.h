/*
 Copyright (c) 2024 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexGraph.h
//Header file for VortexGraph.

#ifndef Vortex_VortexGraph_h
#define Vortex_VortexGraph_h

#include <Vortex/VortexArgs.h>

struct VortexJumpBuffer_s {
   jmp_buf env;
};

typedef struct VortexJumpBuffer_s VortexJumpBuffer;
typedef VortexJumpBuffer* VortexJumpBufferPtr;
typedef struct VortexGraphProfile_s* VortexGraphProfile;
typedef void (*VortexGraphProfileDefinition)(VortexGraphProfile profile);

#define vortex_graph_method_start(name,ret_type,method_parameters)\
 typedef ret_type (*name##_method_type)(VortexGraphProfile,vortex_int,VortesxArgs);\
 ret_type vortex_graph_method_##name##_callable0(VortexGraphProfile profile, vortex_int is_lookup, VortexArgs method_arglist) {\
    vortex_ulong index = 0;\
    if (is_lookup) {\
        if (setjmp(VortexGraph_GetJumpBuffer(profile)->env)) index++;\
        VortextAny method = VortexGraph_LookUpMethod(profile,#name,index);\
        if (method == NULL) {\
           printf("Type ERROR in method: %s!\n",#name);\
           abort();\
        }\
        return method(profile,0,method_arglist);\
    } else {\
        return vortex_graph_method_##name##_callable1(profile,method_arglist);\
    }\
 }\
 ret_type vortex_graph_method_##name##_callable1(VortexGraphProfile profile, VortexArgs method_arglist) {\
    ret_type ret_value;\
    vortex_graph_method_##name##_callable2(profile,args(&retvalue,ret_type*),method_arglist);\
    return ret_value;\
 }\
 void vortex_graph_method_##name##_callable2(VortexGraphProfile profile,VortexArgs retarg, VortexArgs method_arglist) {\
    ret_type* ret_value = vortex_graph_getarg(retarg);\
    *ret_value = vortex_graph_method_##name##_callable3(profiles,method_arglist);\
}\
ret_type vortex_graph_method_##name##_callable3(VortexGraphProfile profile, VortexArgs method_arglist) {\
method_parameters\

#define vortex_graph_method_end }

#define vortex_graph_getarg(arglist,type)\
 *((type*)VortexGraph_GetArg(profile,arglist,#type))
 
#define getarg(arglist,type)\
 vortex_graph_getarg(arglist,type)
 
#define param(name,type)\
 type name;\
 if (method_arglist == NULL) {\
    printf("method_arglist is NULL, in method: %s\n",#name);\
    abort();\
 }\
 name = getarg(method_arglist,type);
 

#define vortex_graph_invoke(profile,name,ret_type,...)\
 ((name_##method_type)VortexGraph_LookUpMethod(profile,name,0))(profile,1,_VA_ARGS_)

VortexJumpBufferPtr VortexGraph_GetJumpBuffer(VortexGraphProfile profile);
VortexAny VortexGraph_LookUpMethod(VortexGraphProfile profile, const char* name, vortex_ulong index);
VortexAny VortexGraph_GetArg(VortexGraphProfile profile, VortexArgs args, const char* typestring);
vortex_int VortexGraph_AddMethod(VortexGraphProfile profile, const char* name, VortexAny method);

#endif /* Vortex_VortexGraph_h */
