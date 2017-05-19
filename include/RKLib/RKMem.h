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

//File: RKMem.h
//Header file for RKMem.

#ifndef RKLib_RKMem_h
#define RKLib_RKMem_h

#include <stdint.h>

typedef float RKFloat ;

typedef double RKDouble ;

typedef unsigned char RKByte ;

typedef signed char RKSByte ;

typedef signed short RKShort ;

typedef unsigned short RKUShort ;

typedef signed int RKInt ;

typedef unsigned int RKUInt ;

#ifdef _WIN32

typedef signed long long RKLong ;

typedef unsigned long long RKULong ;

#else

typedef signed long RKLong ;

typedef unsigned long RKULong ;

#endif

#define RKMem_CArray(size, type) (type*) malloc(sizeof(type) * (size))

#define RKMem_NewMemOfType(type) RKMem_CArray(1,type)

#define RKMem_Realloc(data, newsize, oldsize, type, NULLonError0No1Yes) (type*) RKMem_Realloc_Func( data, sizeof(type) * (newsize), sizeof(type) * (oldsize), NULLonError0No1Yes )

#define RKMem_AddToArray(array_a,array_b,array_a_size,array_b_size,type) array_a = RKMem_Realloc(array_a,(array_a_size+array_b_size),array_a_size,type,1) ; memcpy(array_a[array_a_size+1],array_b,(array_b_size*sizeof(type)))


#define RKString_NewString( string ) RKString_NewStringFromBuffer(string,sizeof(string))

#define rkstr( string ) RKString_NewString(string)

#define rkstrfree( string ) RKString_DestroyString(string)

#define rkstrprint( string ) RKString_PrintString(string)

#define rkany(any) RKAny_NewAny(&any,sizeof(any))

#define rkget(type,any) *((type*)any)

void* RKMem_Realloc_Func(void* data, size_t newsize, size_t oldsize, int NULLonError0No1Yes) ;

typedef void (*RKMemIteratorFuncType)(void* data) ;

typedef void* (*RKList_GetDataFromArrayFuncType)(void* array, int index) ;

typedef struct RKList_node_s* RKList_node ;

typedef struct RKList_s* RKList ;

typedef struct RKStore_s* RKStore ;

typedef struct RKIndex_s* RKIndex ;

typedef struct RKString_s* RKString ;

typedef void* RKAny ;

RKList RKList_NewList( void ) ;

RKList RKList_NewListFromArray( void* array, RKList_GetDataFromArrayFuncType GetDataFromArrayFunc, int size ) ;

RKList_node RKList_AddToList( RKList list, void* data ) ;

RKList_node RKList_AddNodeToList( RKList_node node, RKList list ) ;

void RKList_NodeSwap( RKList list, RKList_node node_a, RKList_node node_b ) ;

void RKList_DataSwap( RKList_node node_a, RKList_node node_b ) ;

void RKList_InsertNodeAToNodeB( RKList list, RKList_node node_a, RKList_node node_b ) ;

RKList_node RKList_MoveNodeFromListToList( RKList_node node, RKList list_a, RKList list_b ) ;

void RKList_CopyList(RKList list_a, RKList list_b) ;

void RKList_CopyToListFromArray(RKList list, void* array, RKList_GetDataFromArrayFuncType GetDataFromArrayFunc, int size) ;

void RKList_SetData(RKList_node node, void* data) ;

void* RKList_GetData(RKList_node node) ;

RKList_node RKList_GetNextNode(RKList_node node) ;

RKList_node RKList_GetPreviousNode(RKList_node node) ;

RKList_node RKList_GetFirstNode(RKList list) ;

RKList_node RKList_GetLastNode(RKList list) ;

int RKList_GetNumOfNodes(RKList list) ;

void RKList_DeleteNode( RKList list, RKList_node node ) ;

RKList_node RKList_DeleteNodeAndReturnNextNode( RKList list, RKList_node node ) ;

void RKList_DeleteList( RKList list ) ;

void RKList_DeleteAllNodesInList( RKList list ) ;

RKList_node RKList_GetNode( RKList list, int index ) ;

void RKList_DeleteNodeWithIndex( RKList list, int index ) ;

void RKList_IterateListWith( RKMemIteratorFuncType iterator, RKList list ) ;

RKStore RKStore_NewStore( void ) ;

int RKStore_AddItem( RKStore store, void* item, const char* label ) ;

int RKStore_RemoveItem( RKStore store, const char* label ) ;

void* RKStore_GetItem( RKStore store, const char* label ) ;

int RKStore_ItemExists( RKStore store, const char* label ) ;

int RKStore_AddItemToList( RKStore store, void *item ) ;

RKList RKStore_GetList( RKStore store ) ;

void RKStore_IterateStoreWith( RKMemIteratorFuncType iterator, RKStore store ) ;

void RKStore_DestroyStore( RKStore store ) ;

RKIndex RKIndex_NewIndex( int max_num_of_items ) ;

void RKIndex_DestroyIndex( RKIndex rkindex ) ;

int RKIndex_AddItem( RKIndex rkindex, void *item ) ;

int RKIndex_SetItem( RKIndex rkindex, void *item, int index ) ;

void* RKIndex_GetItem( RKIndex rkindex, int index ) ;

int RKIndex_GetMaxNumOfItems( RKIndex rkindex ) ;

int RKIndex_GetNumOfItems( RKIndex rkindex ) ;

RKString RKString_NewStringFromBuffer( const char* text, size_t size_in_bytes )  ;

RKString RKString_NewStringFromCString( const char* text ) ;

void RKString_DestroyString( RKString string ) ;

RKULong RKString_GetSize( RKString string ) ;

char* RKString_GetString( RKString string ) ;

char* RKString_ConvertToCString( RKString string ) ;

void RKString_PrintString( RKString string ) ;

void* RKAny_NewAny( void* any, RKULong size ) ;

#endif
