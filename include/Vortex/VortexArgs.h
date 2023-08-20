/*
 Copyright (c) 2016-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexArgs.h
//Header file for VortexArgs.

#ifndef Vortex_VortexArgs_h
#define Vortex_VortexArgs_h

typedef enum { 
 VortexArgs_Dynamic_type,
 VortexArgs_Proxy_type 
} VortexArgsBaseType;

typedef struct VortexArgArray_s { 
  vortex_ulong size_in_bytes;
  vortex_ulong num_of_bytes; 
  VortexAny array; 
} *VortexArgArray;

typedef struct VortexArgsBase_s {
 vortex_int verify;
 VortexArgsBaseType base_type;
 VortexAny type_array;
 VortexArgArray args;
 } *VortexArgsBase;

typedef struct VortexArgs_s {
 vortex_int verify;
 vortex_ulong index;
 vortex_int cloned;
 VortexArgArray array;
 } *VortexArgs;

#define VortexArgs_Dynamic_Type_Protocol vortex_int verify; VortexArgsBaseType base_type; VortexAny type_array
#define VortexArgs_NewArgArray(type,...) ((struct VortexArgArray_s){sizeof(type),sizeof((type[]){__VA_ARGS__}),(type[]){__VA_ARGS__}})
#define VortexArgs_NewArgSet(type,...) (void*)&((struct VortexArgsBase_s){12345423,VortexArgs_Proxy_type,(void*[]){#type,NULL},&(VortexArgs_NewArgArray(type,__VA_ARGS__))})
#define VortexArgs_NewArgs(...) (VortexArgs)&((struct VortexArgs_s){17345432,0,0,(VortexArgArray)&VortexArgs_NewArgArray(void*,__VA_ARGS__)})
#define VortexArgs_UseArgs(args) args->index = 0
#define VortexArgs_GetNextArg(args,type) *((type*)VortexArgs_GetNextArgFunc(args,#type,0))
#define VortexArgs_GetArgWithIndex(args,type,index) *((type*)VortexArgs_GetArgWithIndexFunc(args,index,#type,0))
#define VortexArgs_GetNextItem(args,type) (type)VortexArgs_GetNextArgFunc(args,#type,1)
#define VortexArgs_GetItemWithIndex(args,type,index) (type)VortexArgs_GetArgWithIndexFunc(args,index,#type,1)
#define VortexArgs_CanGetNextArg(args,type) VortexArgs_CanGetNextArgFunc(args,#type,0)
#define VortexArgs_CanGetArgWithIndex(args,type,index) VortexArgs_CanGetArgWithIndexFunc(args,index,#type,0)
#define VortexArgs_CanGetNextItem(args,type) VortexArgs_CanGetNextArgFunc(args,#type,1)
#define VortexArgs_CanGetItemWithIndex(args,type,index) VortexArgs_CanGetArgWithIndexFunc(args,index,#type,1)
#define UseArgs(args) VortexArgs_UseArgs(args)
#define GetNextArg(args,type) VortexArgs_GetNextArg(args,type)
#define GetArgWithIndex(args,type,index) VortexArgs_GetArgWithIndex(args,type,index)
#define GetNextItem(args,type) VortexArgs_GetNextItem(args,type)
#define GetItemWithIndex(args,type,index) VortexArgs_GetItemWithIndex(args,type,index)
#define CanGetNextArg(args,type) VortexArgs_CanGetNextArg(args,type)
#define CanGetArgWithIndex(args,type,index) VortexArgs_CanGetArgWithIndex(args,type,index)
#define CanGetNextItem(args,type) VortexArgs_CanGetNextItem(args,type)
#define CanGetItemWithIndex(args,type,index) VortexArgs_CanGetItemWithIndex(args,type,index)
#define NewArgs(...) VortexArgs_NewArgs(__VA_ARGS__)
#define Args(type,...) VortexArgs_NewArgSet(type,__VA_ARGS__)
#define newargs(...) NewArgs(__VA_ARGS__)
#define args(type,...) Args(type,__VA_ARGS__)
#define chars(...) VortexArgs_NewArgSet(char,__VA_ARGS__)
#define bytes(...) VortexArgs_NewArgSet(vortex_byte,__VA_ARGS__)
#define shorts(...) VortexArgs_NewArgSet(short,__VA_ARGS__)
#define ints(...) VortexArgs_NewArgSet(int,__VA_ARGS__)
#define longs(...) VortexArgs_NewArgSet(long,__VA_ARGS__)
#define floats(...) VortexArgs_NewArgSet(float,__VA_ARGS__)
#define doubles(...) VortexArgs_NewArgSet(double,__VA_ARGS__)
#define strings(...) VortexArgs_NewArgSet(const char*,__VA_ARGS__)
#define Bytes(...) VortexArgs_NewArgSet(vortex_sbyte,__VA_ARGS__)
#define Shorts(...) VortexArgs_NewArgSet(vortex_short,__VA_ARGS__)
#define Ints(...) VortexArgs_NewArgSet(vortex_int,__VA_ARGS__)
#define Longs(...) VortexArgs_NewArgSet(vortex_long,__VA_ARGS__)
#define UBytes(...) VortexArgs_NewArgSet(vortex_byte,__VA_ARGS__)
#define UShorts(...) VortexArgs_NewArgSet(vortex_ushort,__VA_ARGS__)
#define UInts(...) VortexArgs_NewArgSet(vortex_uint,__VA_ARGS__)
#define ULongs(...) VortexArgs_NewArgSet(vortex_ulong,__VA_ARGS__)
#define Floats(...) VortexArgs_NewArgSet(vortex_float,__VA_ARGS__)
#define Doubles(...) VortexArgs_NewArgSet(vortex_double,__VA_ARGS__)

vortex_int VortexArgs_CanGetNextArgFunc(VortexArgs args, const char* typestring, vortex_int is_item);
vortex_int VortexArgs_CanGetArgWithIndexFunc(VortexArgs args, vortex_ulong index, const char* typestring, vortex_int is_item);
VortexAny VortexArgs_GetNextArgFunc(VortexArgs args, const char* typestring, vortex_int is_item);
VortexAny VortexArgs_GetArgWithIndexFunc(VortexArgs args, vortex_ulong index, const char* typestring, vortex_int is_item);
VortexArgs VortexArgs_CloneArgs(VortexArgs args);
vortex_int VortexArgs_DestroyClonedArgs(VortexArgs args);

#endif /* Vortex_VortexArgs_h */
