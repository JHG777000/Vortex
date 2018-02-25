/*
 Copyright (c) 2016-2018 Jacob Gordon. All rights reserved.
 
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

//File: RKArgs.h

#ifndef RKLib_RKArgs_h
#define RKLib_RKArgs_h

typedef enum { RKArgs_Dynamic_type, RKArgs_Proxy_type } RKArgsBaseType ;

typedef struct RKArgArray_s { int size_in_bytes ; int num_of_bytes ; void* array ; } *RKArgArray ;

typedef struct RKArgsBase_s { int verify ; RKArgsBaseType base_type ; void* type_array ; RKArgArray args ; } *RKArgsBase ;

typedef struct RKArgs_s { int verify ; int index ; int cloned ; RKArgArray array ; } *RKArgs ;

#define RKArgs_Dynamic_Type_Protocol int verify ; RKArgsBaseType base_type ; void* type_array

#define RKArgs_NewArgArray(type,...) ((struct RKArgArray_s){sizeof(type),sizeof((type[]){__VA_ARGS__}),(type[]){__VA_ARGS__}})

#define RKArgs_NewArgSet(type,...) (void*)&((struct RKArgsBase_s){12345423,RKArgs_Proxy_type,(void*[]){#type,NULL},&(RKArgs_NewArgArray(type,__VA_ARGS__))})

#define RKArgs_NewArgs(...) (RKArgs)&((struct RKArgs_s){17345432,0,0,(RKArgArray)&RKArgs_NewArgArray(void*,__VA_ARGS__)})

#define RKArgs_UseArgs(args) args->index = 0

#define RKArgs_GetNextArg(args,type) *((type*)RKArgs_GetNextArgFunc(args,#type,0))

#define RKArgs_GetArgWithIndex(args,type,index) *((type*)RKArgs_GetArgWithIndexFunc(args,index,#type,0))

#define RKArgs_GetNextItem(args,type) (type)RKArgs_GetNextArgFunc(args,#type,1)

#define RKArgs_GetItemWithIndex(args,type,index) (type)RKArgs_GetArgWithIndexFunc(args,index,#type,1)

#define RKArgs_CanGetNextArg(args,type) RKArgs_CanGetNextArgFunc(args,#type,0)

#define RKArgs_CanGetArgWithIndex(args,type,index) RKArgs_CanGetArgWithIndexFunc(args,index,#type,0)

#define RKArgs_CanGetNextItem(args,type) RKArgs_CanGetNextArgFunc(args,#type,1)

#define RKArgs_CanGetItemWithIndex(args,type,index) RKArgs_CanGetArgWithIndexFunc(args,index,#type,1)

#define UseArgs(args) RKArgs_UseArgs(args)

#define GetNextArg(args,type) RKArgs_GetNextArg(args,type)

#define GetArgWithIndex(args,type,index) RKArgs_GetArgWithIndex(args,type,index)

#define GetNextItem(args,type) RKArgs_GetNextItem(args,type)

#define GetItemWithIndex(args,type,index) RKArgs_GetItemWithIndex(args,type,index)

#define CanGetNextArg(args,type) RKArgs_CanGetNextArg(args,type)

#define CanGetArgWithIndex(args,type,index) RKArgs_CanGetArgWithIndex(args,type,index)

#define CanGetNextItem(args,type) RKArgs_CanGetNextItem(args,type)

#define CanGetItemWithIndex(args,type,index) RKArgs_CanGetItemWithIndex(args,type,index)

#define NewArgs(...) RKArgs_NewArgs(__VA_ARGS__)

#define Args(type,...) RKArgs_NewArgSet(type,__VA_ARGS__)

#define newargs(...) NewArgs(__VA_ARGS__)

#define args(type,...) Args(type,__VA_ARGS__)

#define chars(...) RKArgs_NewArgSet(char,__VA_ARGS__)

#define bytes(...) RKArgs_NewArgSet(RKByte,__VA_ARGS__)

#define shorts(...) RKArgs_NewArgSet(short,__VA_ARGS__)

#define ints(...) RKArgs_NewArgSet(int,__VA_ARGS__)

#define longs(...) RKArgs_NewArgSet(long,__VA_ARGS__)

#define floats(...) RKArgs_NewArgSet(float,__VA_ARGS__)

#define doubles(...) RKArgs_NewArgSet(double,__VA_ARGS__)

#define strings(...) RKArgs_NewArgSet(const char*,__VA_ARGS__)

#define Bytes(...) RKArgs_NewArgSet(RKSByte,__VA_ARGS__)

#define Shorts(...) RKArgs_NewArgSet(RKShort,__VA_ARGS__)

#define Ints(...) RKArgs_NewArgSet(RKInt,__VA_ARGS__)

#define Longs(...) RKArgs_NewArgSet(RKLong,__VA_ARGS__)

#define UBytes(...) RKArgs_NewArgSet(RKByte,__VA_ARGS__)

#define UShorts(...) RKArgs_NewArgSet(RKUShort,__VA_ARGS__)

#define UInts(...) RKArgs_NewArgSet(RKUInt,__VA_ARGS__)

#define ULongs(...) RKArgs_NewArgSet(RKULong,__VA_ARGS__)

int RKArgs_CanGetNextArgFunc( RKArgs args, const char* typestring, int is_item ) ;

int RKArgs_CanGetArgWithIndexFunc( RKArgs args, int index, const char* typestring, int is_item ) ;

void* RKArgs_GetNextArgFunc( RKArgs args, const char* typestring, int is_item ) ;

void* RKArgs_GetArgWithIndexFunc( RKArgs args, int index, const char* typestring, int is_item ) ;

RKArgs RKArgs_CloneArgs( RKArgs args ) ;

int RKArgs_DestroyClonedArgs( RKArgs args ) ;

#endif /* RKLib_RKArgs_h */
