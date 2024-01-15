/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexMem.h
//Header file for VortexMem.

#ifndef Vortex_VortexMem_h
#define Vortex_VortexMem_h

#include <stdint.h>
#include <limits.h>
#include <stdarg.h>

typedef float vortex_float;
typedef double vortex_double;
typedef unsigned char vortex_byte;
typedef signed char vortex_sbyte;
typedef signed short vortex_short;
typedef unsigned short vortex_ushort;
typedef signed int vortex_int;
typedef unsigned int vortex_uint;
#ifdef _WIN32
 typedef signed long long vortex_long;
 typedef unsigned long long vortex_ulong;
#else
 typedef signed long vortex_long;
 typedef unsigned long vortex_ulong;
#endif

typedef void* VortexAny;

typedef enum {
 VortexArrayFlag,
 VortexListFlag,
 VotrtexStringFlag,
 VortexStoreFlag,
 VortexListNodeFlag,
 VortexParseTreeFlag,
 VortexParseTreeNodeFlag
} VortexDataStructureFlag;

#define vortex_c_array(size, type) (type*) malloc(sizeof(type) * (size))
#define vortex_new_mem_of_type(type) vortex_c_array(1,type)

#define vortex_realloc(data, newsize, oldsize, type, NULLonError0No1Yes) (type*) VortexMem_Realloc( data, sizeof(type) * (newsize), sizeof(type) * (oldsize), NULLonError0No1Yes )
#define vortex_add_to_array(array_a,array_b,array_a_size,array_b_size,type) array_a = vortex_realloc(array_a,(array_a_size+array_b_size),array_a_size,type,1) ; memcpy(array_a[array_a_size+1],array_b,(array_b_size*sizeof(type)))
#define vortex_new_array(array) VortexArray_NewFromBuffer(array,sizeof(array))

#define vortex_array_foreach(index,array) for (vortex_ulong index = 0; index < VortexArray_GetNumofItems(array); index++)
#define vortex_list_foreach(node,list) for (VortexListNode node = VortexList_GetFirstNode(list); node != NULL; node = VortexList_GetNextNode(node))
#define vortex_store_foreach(node,store) vortex_list_foreach(node,VortexStore_GetList(store))

#define VortexString_New(string) VortexString_NewFromBuffer(string,sizeof(string))
#define VortexString_AddStrings(a, b) VortexString_NewFromTwoStrings(a,b)
#define vortex_str(string) VortexString_New(string)
#define vortex_strU8(string) VortexString_New(u8##string)
#define vortex_strfree(string) VortexString_Destroy(string)
#define vortex_strprint(string) VortexString_Print(string)
#define vortex_any(any) VortexAny_New(&any,sizeof(any))
#define vortex_get(type,any) *((type*)any)

VortexAny VortexMem_Realloc(VortexAny data, vortex_ulong newsize, vortex_ulong oldsize, vortex_int NULLonError0No1Yes);

typedef void (*VortexMemIteratorFuncType)(VortexAny data);
typedef VortexAny (*VortexListGetDataFromArrayFuncType)(VortexAny array, vortex_ulong index);

typedef struct VortexDataStructure_s {
 VortexDataStructureFlag flag;
}* VortexDataStructure;

typedef struct VortexListNode_s* VortexListNode;
typedef struct VortexArray_s* VortexArray;
typedef struct VortexList_s* VortexList;
typedef struct VortexStore_s* VortexStore;
typedef struct VortexArrayStore_s* VortexArrayStore;
typedef struct VortexStack_s* VortexStack;
typedef struct VortexString_s* VortexString;

VortexArray VortexArray_New(void);
VortexArray VortexArray_NewWithBaseSize(vortex_ulong base_size);
VortexArray VortexArray_NewFromBuffer(VortexAny buffer[], vortex_ulong size);
void VortexArray_Destroy(VortexArray array);
void VortexArray_AddItem(VortexArray array, VortexAny item);
void VortexArray_AddArray(VortexArray array, VortexAny items[], vortex_ulong num_of_items_to_add);
void VortexArray_AddSpace(VortexArray array, vortex_ulong space_to_add);
VortexAny VortexArray_GetBuffer(VortexArray array);
VortexAny VortexArray_GetItem(VortexArray array, vortex_ulong index);
vortex_int VortexArray_SetItem(VortexArray array, vortex_ulong index, VortexAny item);
VortexArray VortexArray_CopyArray(VortexArray array);
vortex_ulong VortexArray_GetNumofItems(VortexArray array);
void VortexArray_IterateWith(VortexMemIteratorFuncType iterator, VortexArray array);

VortexList VortexList_New(void);
VortexListNode VortexList_NewNode(VortexListNode before, VortexListNode after, VortexAny item);
VortexList VortexList_NewListFromArray(VortexAny array, VortexListGetDataFromArrayFuncType GetDataFromArrayFunc, vortex_ulong size);
void VortexList_Destroy(VortexList list);
void VortexList_DestroyAllNodes(VortexList list);
void VortexList_DestroyNode(VortexList list, VortexListNode node);
VortexListNode VortexList_DestroyNodeAndReturnNextNode(VortexList list, VortexListNode node);
void Vortex_ListDestroyNodeWithIndex(VortexList list, vortex_ulong index);
VortexListNode VortexList_AddToList(VortexList list, VortexAny item);
VortexListNode VortexList_AddNodeToList(VortexListNode node, VortexList list);
void VortexList_NodeSwap(VortexList list, VortexListNode node_a, VortexListNode node_b);
void VortexList_DataSwap(VortexListNode node_a, VortexListNode node_b );
void VortexList_InsertNodeAToNodeB(VortexList list, VortexListNode node_a, VortexListNode node_b);
VortexListNode VortexList_MoveNodeFromListToList(VortexListNode node, VortexList list_a, VortexList list_b);
void VortexList_CopyList(VortexList list_a, VortexList list_b) ;
void VortexList_CopyToListFromArray(VortexList list, VortexAny array, VortexListGetDataFromArrayFuncType GetDataFromArrayFunc, vortex_ulong size);
void VortexList_SetItem(VortexListNode node, VortexAny item);
VortexAny VortexList_GetItem(VortexListNode node);
VortexListNode VortexList_GetNextNode(VortexListNode node);
VortexListNode VortexList_GetPreviousNode(VortexListNode node);
VortexListNode VortexList_GetFirstNode(VortexList list);
VortexListNode VortexList_GetLastNode(VortexList list);
vortex_ulong VortexList_GetNumOfNodes(VortexList list);
VortexListNode VortexList_GetNode(VortexList list, vortex_ulong index);
vortex_ulong VortexList_GetIndex(VortexList list, VortexListNode node);
VortexListNode VortexList_GetNextNodeAfterN(VortexListNode node, vortex_ulong n);
VortexListNode VortexList_GetPreviousNodeAfterN(VortexListNode node, vortex_ulong n);
void VortexList_IterateWith(VortexMemIteratorFuncType iterator, VortexList list);

VortexStore VortexStore_New(void);
void VortexStore_Destroy(VortexStore store);
vortex_int VortexStore_AddItem(VortexStore store, void* item, const char* label);
vortex_int VortexStore_RemoveItem(VortexStore store, const char* label);
VortexAny VortexStore_GetItem(VortexStore store, const char* label);
VortexAny VortexStore_GetItemWithCharacter(VortexStore store, vortex_int character);
vortex_int VortexStore_ItemExists(VortexStore store, const char* label);
vortex_ulong VortexStore_GetNumOfItems(VortexStore store);
vortex_int VortexStore_AddItemToList(VortexStore store, void *item);
VortexList VortexStore_GetList(VortexStore store);
VortexString VortexStore_GetLabelFromListNode(VortexListNode node);
void VortexStore_IterateWith(VortexMemIteratorFuncType iterator, VortexStore store);
vortex_int VortexStore_IsEmpty(VortexStore store);


VortexArrayStore VortexArrayStore_New(vortex_ulong max_num_of_items);
void VortexArrayStore_Destroy(VortexArrayStore array_store);
vortex_int VortexArrayStore_AddItem(VortexArrayStore array_store, VortexAny item);
vortex_int VortexArrayStore_SetItem(VortexArrayStore array_store, VortexAny item, vortex_ulong index);
VortexAny VortexArrayStore_GetItem(VortexArrayStore array_store, vortex_ulong index);
vortex_int VortexArrayStore_RemoveItem(VortexArrayStore array_store, vortex_ulong index);
vortex_ulong VortexArrayStore_GetMaxNumOfItems(VortexArrayStore array_store);
vortex_ulong VortexArrayStore_GetNumOfItems(VortexArrayStore array_store);
vortex_int VortexArrayStore_IsEmpty(VortexArrayStore array_store);

VortexString VortexString_NewEmptyString(vortex_ulong size_in_bytes);
VortexString VortexString_NewFromUTF32(const vortex_int* text, vortex_ulong num_of_characters);
VortexString VortexString_NewFromBuffer(const char* text, vortex_ulong size_in_bytes);
VortexString VortexString_NewFromCString(const char* text);
VortexString VortexString_NewFromTwoStrings(VortexString a, VortexString b);
void VortexString_Destroy(VortexString string);
void VortexString_FormatToString(VortexString string, const char* format,...);
vortex_ulong VortexString_GetSize(VortexString string);
vortex_ulong VortexString_GetSizeInBytes(VortexString string);
vortex_ulong VortexString_GetLength(VortexString string);
char* VortexString_GetBuffer(VortexString string);
vortex_int VortexString_GetCharacter(VortexString string, vortex_ulong index, vortex_ulong* offset);
vortex_byte VortexString_GetByte(VortexString string, vortex_ulong index);
void VortexString_SetByte(VortexString string, vortex_ulong index, vortex_byte byte);
VortexString VortexString_AppendString(VortexString BaseString, VortexString AppendingString);
VortexString VortexString_Copy(VortexString string);
vortex_int VortexString_AreStringsEqual(VortexString a, VortexString b);
char* VortexString_ConvertToCString(VortexString string);
void VortexString_Print(VortexString string);
vortex_int* VortexString_GetUTF32String(VortexString string, vortex_ulong* length);
VortexString VortexString_GetStringForASCII(VortexString string);
VortexString VortexString_SwapEscapeSequencesWithCharacters(VortexString string);

VortexAny VortexAny_New(VortexAny any, vortex_ulong size);

VortexStack VortexStack_New(void);
void VortexStack_Destroy(VortexStack stack);
void VortexStack_Push(VortexStack stack, VortexAny item);
VortexAny VortexStack_Pop(VortexStack stack);
VortexAny VortexStack_Peek(VortexStack stack);
vortex_int VortexStack_IsEmpty(VortexStack stack);
VortexList VortexStack_GetList(VortexStack stack);
VortexListNode VortexStack_GetListNode(VortexStack stack);

#endif /* Vortex_VortexMem_h */
