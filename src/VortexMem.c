/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexMem.c
//VortexMem, provides some nice macros and a funtion to make dealing with memory a little easier in C.
//As well as a double linked-list implementation, VortexList.
//And a dictionary type, VortexStore.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Vortex/VortexMem.h>

struct VortexArray_s { 
  VortexAny* items;
  vortex_ulong num_of_items;
};
 
struct VortexStack_s { 
  VortexList list;
};

struct VortexString_s { 
  vortex_ulong size_in_characters; 
  vortex_ulong size_in_bytes; 
  char* buffer;
};

struct VortexArrayStore_s { 
  VortexStore store; 
  vortex_ulong max_num_of_items; 
  vortex_ulong num_of_items;
};

typedef struct VortexStoreLetter_s { 
  VortexListNode node; 
  struct VortexStoreLetter_s* next_alphabet; 
} VortexStoreLetter;

struct VortexStore_s {
  VortexStoreLetter* dictionary; 
  VortexList items;
};

struct VortexListNode_s { 
  struct VortexListNode_s* before;
  struct VortexListNode_s* after;
  VortexAny data; 
  VortexString string;
};

struct VortexList_s {
  vortex_ulong num_of_nodes;
  VortexListNode first; 
  VortexListNode last;
};

VortexAny VortexMem_Realloc(VortexAny data, vortex_ulong newsize, 
    vortex_ulong oldsize, vortex_int NULLonError0No1Yes) {
  VortexAny newdata;
  newdata = malloc(newsize);
  if (newdata != NULL) {
    memcpy(newdata, data, oldsize);
     free(data);
    return newdata;
    } else {
        if (NULLonError0No1Yes) {
            free(data);
            return NULL;
        } else {
            return data;
        }
    }
}

VortexArray VortexArray_New(void) {
  VortexArray array = vortex_new_mem_of_type(struct VortexArray_s);
  array->num_of_items = 0;
  array->items = NULL;
  return array;
}

VortexArray VortexArray_NewWithBaseSize(vortex_ulong base_size) {
  VortexArray array = vortex_new_mem_of_type(struct VortexArray_s) ;
  array->items = vortex_c_array(base_size, VortexAny) ;
  array->num_of_items = base_size;
  return array;
}

VortexArray VortexArray_NewFromBuffer(VortexAny buffer[], vortex_ulong size) {
  VortexArray array = VortexArray_NewWithBaseSize(size);
  memcpy(array->items,buffer,array->num_of_items);
  return array;
}

void VortexArray_Destroy(VortexArray array) {
  free(array->items);
  free(array);
}

void VortexArray_AddItem(VortexArray array, VortexAny item) {
  array->num_of_items++;
  if (array->items == NULL) {
   array->items = vortex_c_array(array->num_of_items, VortexAny);
  } else {
   array->items = vortex_realloc(array->items,
    array->num_of_items,
    array->num_of_items-1,
    void*,
    1);
 }
 array->items[array->num_of_items-1] = item;
}

void VortexArray_AddArray(VortexArray array, VortexAny items[],
     vortex_ulong num_of_items_to_add) {
  VortexArray_AddSpace(array,num_of_items_to_add);
  memcpy(array->items+(array->num_of_items-num_of_items_to_add),items,array->num_of_items);
}

void VortexArray_AddSpace(VortexArray array, vortex_ulong space_to_add) {
  if ( array->items == NULL ) {
   array->num_of_items = space_to_add;
   array->items = vortex_c_array(array->num_of_items, VortexAny);
  } else {
   array->num_of_items += space_to_add;
   array->items = vortex_realloc(array->items,
    array->num_of_items,
    array->num_of_items-space_to_add,
    VortexAny,
    1);

 }

}

void* VortexArray_GetBuffer(VortexArray array) {
  return array->items;  
}

VortexAny VortexArray_GetItem(VortexArray array, vortex_ulong index) {
  if ( index >= 0 && index < array->num_of_items ) {
    return array->items[index];
  }
  return NULL;
}

vortex_int VortexArray_SetItem(VortexArray array, vortex_ulong index, VortexAny item) {
  if ( index >= 0 && index < array->num_of_items ) {
    array->items[index] = item;
    return 1;
  }
  return 0;
}

vortex_ulong VortexArray_GetNumofItems(VortexArray array) {
   return array->num_of_items;
}

void VortexArray_IterateWith(VortexMemIteratorFuncType iterator, VortexArray array) {
    vortex_ulong i = 0 ;
    while ( i < array->num_of_items ) {
     iterator(array->items[i]) ;
     i++ ;
    }
}

VortexList VortexList_New(void) {
    VortexList newlist = vortex_new_mem_of_type(struct VortexList_s);
    newlist->num_of_nodes = 0;
    newlist->first = NULL;
    newlist->last = NULL;
    return newlist;
}

VortexListNode VortexList_NewNode(VortexListNode before, 
      VortexListNode after, VortexAny item) {
    VortexListNode node = vortex_new_mem_of_type(struct VortexListNode_s);
    node->before = before;
    node->after = after;
    node->data = item;
    node->string = NULL;
    return node;
}

VortexList VortexList_NewFromArray(VortexAny array, 
      VortexListGetDataFromArrayFuncType GetDataFromArrayFunc, vortex_ulong size) {
    VortexList newlist = VortexList_New();
    VortexList_CopyToListFromArray(newlist, array, GetDataFromArrayFunc, size);
    return newlist;
}

VortexListNode VortexList_AddToList(VortexList list, VortexAny item) {
    if ( list->num_of_nodes == 0 ) {
        list->first = vortex_new_mem_of_type(struct VortexListNode_s);
        list->first->before = NULL;
        list->first->after = NULL;
        list->last = list->first;
    } else {
        list->last->after = vortex_new_mem_of_type(struct VortexListNode_s) ;
        list->last->after->before = list->last;
        list->last->after->after = NULL;
        list->last = list->last->after;
    }
    list->last->data = item;
    list->last->string = NULL;
    list->num_of_nodes++;
    return list->last;
}

VortexListNode VortexList_AddNodeToList(VortexListNode node, VortexList list) {
    return VortexList_AddToList(list, node->data);
}

void VortexList_NodeSwap( VortexList list, VortexListNode node_a, VortexListNode node_b ) {
    VortexListNode before = (node_a->before == node_b) ? node_a : node_a->before;
    VortexListNode after = (node_a->after == node_b) ? node_a : node_a->after;
    node_a->before = (node_b->before == node_a) ? node_b : node_b->before;
    node_a->after = (node_b->after == node_a) ? node_b : node_b->after;
    if ( node_a->before != NULL ) node_a->before->after = node_a;
    if ( node_a->after != NULL ) node_a->after->before = node_a;
    if ( node_a->before == NULL ) list->first = node_a;
    if ( node_a->after == NULL ) list->last = node_a;
    node_b->before = before;
    node_b->after = after;
    if ( node_b->before != NULL ) node_b->before->after = node_b;
    if ( node_b->after != NULL ) node_b->after->before = node_b;
    if ( node_b->before == NULL ) list->first = node_b;
    if ( node_b->after == NULL ) list->last = node_b;
}

void VortexList_DataSwap(VortexListNode node_a, VortexListNode node_b) {
    VortexAny data = node_a->data;
    node_a->data = node_b->data;
    node_b->data = data;
}

void VortexList_InsertNodeAToNodeB(VortexList list, VortexListNode node_a, VortexListNode node_b) {
    if ( node_a->before != NULL ) node_a->before->after = node_a->after;
    if ( node_a->after != NULL ) node_a->after->before = node_a->before;
    if ( node_a->before == NULL ) list->first = node_a->after;
    if ( node_a->after == NULL ) list->last = node_a->before;
    if ( node_b->before == NULL ) list->first = node_a;
    if ( node_b->after == NULL ) list->last = node_a;
    node_a->before = node_b->before;
    node_a->after = node_b;
    if ( node_b->before != NULL ) node_b->before->after = node_a;
    node_b->before = node_a;
}

VortexListNode VortexList_MoveNodeFromListToList(VortexListNode node, VortexList list_a, VortexList list_b) {
    VortexListNode new_node = VortexList_AddNodeToList(node, list_b);
    VortexList_DestroyNode(list_a, node);
    return new_node;
}

void VortexList_Copy(VortexList list_a, VortexList list_b) {
    VortexListNode node = VortexList_GetFirstNode(list_a);
    while ( node != NULL ) {
        VortexList_AddNodeToList(node, list_b);
        node = VortexList_GetNextNode(node);
    }
}

void VortexList_CopyToListFromArray(VortexList list, VortexAny array, 
      VortexListGetDataFromArrayFuncType GetDataFromArrayFunc, vortex_ulong size) {
    VortexAny data = NULL;
    vortex_ulong i = 0;
    while ( i < size ) {
        data = GetDataFromArrayFunc(array,i);
        VortexList_AddToList(list, data);
        i++ ;
    }

}

void VortexList_SetData(VortexListNode node, VortexAny data) {
    node->data = data;
}

VortexAny VortexList_GetData(VortexListNode node) {
    if ( node == NULL ) return NULL;
    return node->data;
}

static void Vortex_SetStringForListNode(VortexListNode node, VortexString string) {
    node->string = string ;
}

static VortexString Vortex_GetStringFromListNode(VortexListNode node) {
    if ( node == NULL ) return NULL;
    return node->string;
}

VortexListNode VortexList_GetNextNode(VortexListNode node) {
    if ( node == NULL ) return NULL;
    return node->after;
}

VortexListNode VortexList_GetPreviousNode(VortexListNode node) {
    if ( node == NULL ) return NULL;
    return node->before;
}

VortexListNode VortexList_GetFirstNode(VortexList list) {
    if ( list == NULL ) return NULL;
    return list->first;
}

VortexListNode VortexList_GetLastNode(VortexList list) {
    if ( list == NULL ) return NULL;
    return list->last;
}

vortex_ulong VortexList_GetNumOfNodes(VortexList list) {
    if ( list == NULL ) return -1;
    return list->num_of_nodes;
}

void VortexList_DestroyNode( VortexList list, VortexListNode node) {
    if ( node == NULL ) return;
    if ( ( node->before == NULL ) || ( node->after == NULL ) ) {
     if ( ( node->before == NULL ) && ( node->after == NULL ) ) {
         list->first = NULL;
         list->last = NULL;
     }
     if ( ( node->before == NULL ) && ( node->after != NULL ) ) {
         list->first = node->after;
         list->first->before = NULL;
     }
     if ( ( node->before != NULL ) && ( node->after == NULL ) ) {
         list->last = node->before;
         list->last->after = NULL;
     }
    }
    if ( ( node->before != NULL ) && ( node->after != NULL ) ) {
        node->before->after = node->after;
        node->after->before = node->before;
    }
    list->num_of_nodes--;
    if ( node->string != NULL ) VortexString_Destroy(node->string);
    free(node) ;
}

VortexListNode VortexList_DestroyNodeAndReturnNextNode(VortexList list, VortexListNode node) {
    VortexListNode next_node = VortexList_GetNextNode(node);
    VortexList_DestroyNode(list, node);
    return next_node;
}

void VortexList_Destroy(VortexList list) {
    while ( list->first != NULL ) {
        VortexList_DestroyNode(list,list->first);
    }
    free(list);
}

void VortexList_DestroyAllNodes( VortexList list ) {
    while ( list->first != NULL ) {
        VortexList_DestroyNode(list,list->first) ;
    }
}

VortexListNode VortexList_GetNode(VortexList list, vortex_ulong index) {
    vortex_ulong i = 1;
    VortexListNode node = list->first;
    while ( i < index ) {
        if ( node == NULL ) return NULL;
        node = node->after;
        i++;
    }
    return node;
}

vortex_ulong VortexList_GetIndex(VortexList list, VortexListNode node) {
    vortex_ulong i = 0 ;
    VortexListNode node2 = list->first;
    while ( node2 != NULL ) {
        if ( node == node2 ) break;
        node2 = node2->after;
        i++;
    }
    return i;
}

VortexListNode VortexList_GetNextNodeAfterN(VortexListNode node, vortex_ulong n) {
    vortex_ulong i = 0;
    while ( i < n ) {
        if ( node == NULL ) return NULL;
        node = node->after;
        i++;
    }
    return node;
}

VortexListNode VortexList_GetPreviousNodeAfterN(VortexListNode node, vortex_ulong n) {
    vortex_ulong i = 0;
    while ( i < n ) {
        if ( node == NULL ) return NULL;
        node = node->before;
        i++ ;
    }
    return node;
}

void VortexList_DestroyNodeWithIndex(VortexList list, vortex_ulong index) {
    VortexListNode node = VortexList_GetNode(list, index);
    VortexList_DestroyNode(list, node);
}

void VortexList_IterateWith(VortexMemIteratorFuncType iterator, VortexList list) {
    VortexListNode node = list->first;
    VortexAny data = NULL;
    while ( node != NULL ) {
        data = VortexList_GetData(node);
        iterator(data);
        node = VortexList_GetNextNode(node) ;
    }
}

static VortexStoreLetter* Vortex_NewStoreLetter(void) {
    VortexStoreLetter* alphabet = NULL;
    vortex_int size = 2;
    alphabet = vortex_c_array(size, VortexStoreLetter);
    vortex_int i = 0;
    while ( i < size ) {
        alphabet[i].node = NULL;
        alphabet[i].next_alphabet = NULL;
        i++ ;
    }
    return alphabet;
}

static void Vortex_DestroyStoreLetters(VortexStoreLetter* alphabet) {
    vortex_int i = 0 ;
    while ( i < 2 ) {
        if (alphabet[i].next_alphabet != NULL)  {
            Vortex_DestroyStoreLetters(alphabet[i].next_alphabet);
            alphabet[i].next_alphabet = NULL ;
        }
        i++ ;
    }
    free(alphabet) ;
}

void VortexStore_Destroy(VortexStore store) {
    if (store->dictionary != NULL) Vortex_DestroyStoreLetters(store->dictionary);
    if (store->items != NULL) VortexList_Destroy(store->items);
    free(store) ;
}

static VortexListNode Vortex_GetSetNodeForStore(VortexStore store, const char* label, 
      VortexListNode node, vortex_int flag, vortex_int find ) {
    vortex_int i = 0;
    vortex_int j = 0;
    vortex_int value = 0;
    signed char byte = 0;
    vortex_int size = (int) strlen(label);
    vortex_int size2 = sizeof(char) * CHAR_BIT;
    VortexListNode retnode = NULL;
    VortexStoreLetter* current_alphabet = NULL;
    if ( store->dictionary == NULL ) {
        store->dictionary = Vortex_NewStoreLetter();
    }
    current_alphabet = store->dictionary;
    while ( i < size ) {
        byte = label[i];
        j = 0;
        while ( j < size2 ) {
            value = (byte < 0) ? 1 : 0;
            byte = byte << 1;
            if ( i == ( size - 1 ) && j == ( size2 - 1 ) ) {
                if ( node == NULL ) {
                    if (!flag) {
                        return current_alphabet[value].node;
                    } else {
                        retnode = current_alphabet[value].node;
                        VortexString_Destroy(Vortex_GetStringFromListNode(current_alphabet[value].node));
                        Vortex_SetStringForListNode(current_alphabet[value].node, NULL);
                        current_alphabet[value].node = NULL;
                        return retnode ;
                    }
                } else {
                    current_alphabet[value].node = node;
                    Vortex_SetStringForListNode(current_alphabet[value].node, VortexString_NewFromCString(label)) ;
                    return current_alphabet[value].node;
                }
            } else {
                if ( !find ) if ( current_alphabet[value].next_alphabet == NULL ) 
                  current_alphabet[value].next_alphabet = Vortex_NewStoreLetter();
                if ( find ) if ( current_alphabet[value].next_alphabet == NULL ) return NULL;
                current_alphabet = current_alphabet[value].next_alphabet;

            }
            j++;
        }
        i++;
    }
    return NULL;
}

VortexStore VortexStore_New(void) {
    VortexStore store = vortex_new_mem_of_type(struct VortexStore_s);
    store->dictionary = NULL;
    store->items = NULL;
    return store;
}

static VortexListNode Vortex_AddItemToStore(VortexStore store, VortexAny item) {
    if ( store->items == NULL ) store->items = VortexList_New();
    return VortexList_AddToList(store->items, item);
}

static vortex_int Vortex_StoreItem(VortexStore store, VortexAny item, const char* label) {
    VortexListNode node = Vortex_GetSetNodeForStore(store, label, NULL, 0, 1);
    if ( node == NULL ) return 0;
    VortexList_SetData(node, item);
    return 1 ;
}

vortex_int VortexStore_AddItem(VortexStore store, VortexAny item, const char* label) {
    if ( !(VortexStore_ItemExists(store, label)) ) {
        VortexListNode node = Vortex_AddItemToStore(store, item);
        node = Vortex_GetSetNodeForStore(store, label, node, 0, 0);
        if ( node == NULL ) return 0;
    } else {
        return Vortex_StoreItem(store, item, label);
    }
    return 1;
}

vortex_int VortexStore_RemoveItem(VortexStore store, const char* label) {
    VortexListNode node = Vortex_GetSetNodeForStore(store, label, NULL, 1, 1);
    if ( node == NULL ) return 0;
    VortexList_DestroyNode(store->items, node);
    return 1;
}

VortexAny VortexStore_GetItem(VortexStore store, const char* label) {
    VortexListNode node = Vortex_GetSetNodeForStore(store, label, NULL, 0, 1);
    if ( node == NULL ) return NULL;
    return VortexList_GetData(node);
}

vortex_int VortexStore_ItemExists(VortexStore Store, const char* label) {
    VortexListNode node = Vortex_GetSetNodeForStore(Store, label, NULL, 0, 1);
    if ( node != NULL ) return 1;
    return 0;
}

vortex_ulong VortexStore_GetNumOfItems(VortexStore Store) {
    return VortexList_GetNumOfNodes(VortexStore_GetList(Store)) ;
}

vortex_int VortexStore_AddItemToList(VortexStore store, VortexAny item) {
    if ( Vortex_AddItemToStore(store, item) == NULL ) return 0;
    return 1 ;
}

VortexList VortexStore_GetList(VortexStore store) {
    if ( store->items == NULL ) store->items = VortexList_New(); 
    return store->items;
}

VortexString VortexStore_GetStoreLabelFromListNode(VortexListNode node) {
    if ( node == NULL ) return NULL;
    return Vortex_GetStringFromListNode(node);
}

void VortexStore_IterateWith( VortexMemIteratorFuncType iterator, VortexStore store ) {
    if (store->items != NULL) VortexList_IterateWith(iterator, store->items);
}

vortex_int VortexStore_IsEmpty( VortexStore store ) {
    return (VortexList_GetNumOfNodes(store->items) == 0) ? 1 : 0;
}

VortexArrayStore VortexArrayStore_New(vortex_ulong max_num_of_items) {
    VortexArrayStore array_store = vortex_new_mem_of_type(struct VortexArrayStore_s);
    array_store->max_num_of_items = max_num_of_items;
    array_store->store = VortexStore_New();
    array_store->num_of_items = 0;
    return array_store;
}

void VortexArrayStore_Destroy(VortexArrayStore array_store) {
    VortexStore_Destroy(array_store->store);
    free(array_store);
}

vortex_int VortexArrayStore_AddItem(VortexArrayStore array_store, VortexAny item) {
    char string[100];
    vortex_ulong index = array_store->num_of_items;
    if ( array_store->max_num_of_items > 0 ) {
        if ( index > array_store->max_num_of_items ) return 0;
    }
    if ( index < 0 ) return 0;
    snprintf(string, 100, "%llu", index);
    if ( VortexStore_ItemExists(array_store->store, string) ) return 0;
    array_store->num_of_items++;
    return VortexStore_AddItem(array_store->store, item, string);
}

vortex_int VortexArrayStore_SetItem(VortexArrayStore array_store, void *item, vortex_ulong index) {
    char string[100];
    if ( array_store->max_num_of_items > 0 ) {
        if ( index > array_store->max_num_of_items ) return 0;
    }
    if ( index < 0 ) return 0;
    snprintf(string, 100, "%llu", index);
    if ( !VortexStore_ItemExists(array_store->store, string) ) return 0;
    return VortexStore_AddItem(array_store->store, item, string);
}

VortexAny VortexArrayStore_GetItem(VortexArrayStore array_store, vortex_ulong index) {
    char string[100];
    if ( array_store->max_num_of_items > 0 ) {
        if ( index > array_store->max_num_of_items ) return NULL;
    }
    if ( array_store < 0 ) return NULL;
    snprintf(string, 100, "%llu", index);
    return VortexStore_GetItem(array_store->store, string);
}

vortex_int VortexArrayStore_RemoveItem(VortexArrayStore array_store, vortex_ulong index) {
    char string[100];
    if ( array_store->max_num_of_items > 0 ) {
        if ( index > array_store->max_num_of_items ) return 0;
    }
    if ( index < 0 ) return 0;
    array_store->num_of_items--;
    snprintf(string, 100, "%llu", index);
    return VortexStore_RemoveItem(array_store->store, string);
}

vortex_ulong VortexArrayStore_GetMaxNumOfItems(VortexArrayStore array_store) {
    return array_store->max_num_of_items;
}

vortex_ulong VortexArrayStore_GetNumOfItems(VortexArrayStore array_store) {
    return array_store->num_of_items;
}

vortex_int VortexArrayStore_IsEmpty(VortexArrayStore array_store) {
    return VortexStore_IsEmpty(array_store->store);
}

VortexString VortexString_NewEmptyString(vortex_ulong size_in_bytes) {
    VortexString string = vortex_new_mem_of_type(struct VortexString_s);
    string->size_in_bytes = size_in_bytes;
    string->buffer = vortex_c_array(string->size_in_bytes, char);
    string->buffer[string->size_in_bytes-1] = '\0';
    string->size_in_bytes -= 1; //not counting '\0'
    string->size_in_characters = -1;
    return string;
}

VortexString VortexString_NewFromUTF32(const vortex_int* text, vortex_ulong num_of_characters) {
    vortex_ulong i = 0;
    vortex_uint character = 0;
    vortex_uint value = 0;
    vortex_uint value0 = 0;
    vortex_uint value1 = 0;
    vortex_uint value2 = 0;
    VortexString string = NULL;
    vortex_ulong utf8string_size = 1;
    char* utf8string = vortex_c_array(1, char);
    utf8string[utf8string_size-1] = '\0';
    while ( i < num_of_characters ) {
        character = text[i];
        if ( character >= 0 && character <= 0x007F ) {
            utf8string = vortex_realloc(utf8string, utf8string_size+1, utf8string_size, char, 1);
            utf8string_size++;
            utf8string[utf8string_size-2] = character;
            utf8string[utf8string_size-1] = '\0';
        }
        if ( character >= 0x0080 && character <= 0x07FF ) {
            value = character;
            value = value >> 6;
            value = value | 0xC0;
            value0 = character;
            value0 = value0 & 0x3F;
            value0 = value0 | 0x80;
            utf8string = vortex_realloc(utf8string, utf8string_size+2, utf8string_size, char, 1);
            utf8string_size+=2;
            utf8string[utf8string_size-3] = value;
            utf8string[utf8string_size-2] = value0;
            utf8string[utf8string_size-1] = '\0';
        }
        if ( character >= 0x0800 && character <= 0xFFFF ) {
            value = character;
            value = value >> 12;
            value = value | 0xE0;
            value0 = character;
            value0 = value0 >> 6;
            value0 = value0 & 0xBF;
            value0 = value0 | 0x80;
            value1 = character;
            value1 = value1 & 0x3F;
            value1 = value1 | 0x80;
            utf8string = vortex_realloc(utf8string, utf8string_size+3, utf8string_size, char, 1);
            utf8string_size+=3;
            utf8string[utf8string_size-4] = value;
            utf8string[utf8string_size-3] = value0;
            utf8string[utf8string_size-2] = value1;
            utf8string[utf8string_size-1] = '\0';
        }
        if ( character >= 0x10000 && character <= 0x10FFFF ) {
            value = character;
            value = value >> 18;
            value = value | 0xF0;
            value0 = character;
            value0 = value0 >> 12;
            value0 = value0 & 0xBF;
            value0 = value0 | 0x80;
            value1 = character;
            value1 = value1 >> 6;
            value1 = value1 & 0xBF;
            value1 = value1 | 0x80;
            value2 = character;
            value2 = value2 & 0x3F;
            value2 = value2 | 0x80;
            utf8string = vortex_realloc(utf8string, utf8string_size+4, utf8string_size, char, 1);
            utf8string_size+=4;
            utf8string[utf8string_size-5] = value;
            utf8string[utf8string_size-4] = value0;
            utf8string[utf8string_size-3] = value1;
            utf8string[utf8string_size-2] = value2;
            utf8string[utf8string_size-1] = '\0';
        }
        i++;
    }
    string = VortexString_NewFromCString(utf8string) ;
    free(utf8string);
    return string;
}

VortexString VortexString_NewFromBuffer(const char* text, vortex_ulong size_in_bytes) {
    VortexString string = vortex_new_mem_of_type(struct VortexString_s);
    string->size_in_bytes = size_in_bytes;
    string->buffer = vortex_c_array(string->size_in_bytes, char);
    string->buffer[string->size_in_bytes-1] = '\0';
    vortex_ulong i = 0;
    while ( i < string->size_in_bytes-1 ) {
        string->buffer[i] = text[i];
        i++ ;
    }
    string->size_in_bytes -= 1; //not counting '\0'
    string->size_in_characters = -1;
    return string;
}

VortexString VortexString_NewFromCString(const char* text) {
    VortexString string = vortex_new_mem_of_type(struct VortexString_s);
    string->size_in_bytes = strlen(text);
    string->buffer = vortex_c_array(string->size_in_bytes+1, char);
    strcpy(string->buffer, text);
    string->size_in_characters = -1;
    return string;
}

VortexString VortexString_NewFromTwoStrings(VortexString a, VortexString b) {
    VortexString string = vortex_new_mem_of_type(struct VortexString_s);
    string->size_in_bytes = a->size_in_bytes + b->size_in_bytes;
    string->buffer = vortex_c_array(string->size_in_bytes+1, char);
    vortex_ulong i = 0;
    vortex_ulong j = 0;
    while ( j < a->size_in_bytes ) {
        string->buffer[i] = a->buffer[j];
        i++;
        j++;
    }
    j = 0;
    while ( j < b->size_in_bytes ) {
        string->buffer[i] = b->buffer[j];
        i++;
        j++;
    }
    string->buffer[i] = '\0';
    string->size_in_characters = -1;
    return string;
}

void VortexString_Destroy(VortexString string) {
    if ( string == NULL ) return;
    free(string->buffer);
    free(string);
}

void VortexString_FormatToString(VortexString string, const char* format,...) {
    va_list args;
    va_start(args, format);
    vsnprintf(VortexString_GetBuffer(string),
         VortexString_GetSizeInBytes(string), format, args);
    va_end(args);
}

vortex_ulong VortexString_GetSize(VortexString string) {
    return string->size_in_bytes;
}

vortex_ulong VortexString_GetSizeInBytes(VortexString string) {
    return string->size_in_bytes+1;
}

vortex_ulong VortexString_GetLength(VortexString string) {
    vortex_ulong i = 0 ;
    vortex_ulong n = 0 ;
    vortex_sbyte byte = 0;
    vortex_ulong num_of_characters = 0;
    if ( string->size_in_characters == -1 ) {
        while ( i < string->size_in_bytes ) {
            byte = string->buffer[i];
            if ( byte >= 0 ) {
                if ( byte > 0 ) num_of_characters++ ;
            } else {
                n = 0;
                while (byte < 0) {
                    byte = byte << 1;
                    n++;
                }
                i += (n-1);
                num_of_characters++;
            }
            i++;
        }
        string->size_in_characters = num_of_characters;
    }
    return string->size_in_characters;
}

char* VortexString_GetBuffer(VortexString string) {
    if ( string == NULL ) return NULL;
    return string->buffer;
}

vortex_int VortexString_GetCharacter(VortexString string, vortex_ulong index, vortex_ulong* offset) {
    vortex_ulong n = 0;
    vortex_sbyte byte = 0;
    vortex_uint byte0 = 0;
    vortex_uint byte1 = 0;
    vortex_uint byte2 = 0;
    vortex_uint byte3 = 0;
    vortex_uint value = 0;
loop:
    if ( index > string->size_in_bytes ) return 0;
    if ( index < 0 ) return 0;
    byte = string->buffer[index];
    if ( byte > 0 ) {
        n = 1;
    } else {
        n = 0;
        while (byte < 0) {
            byte = byte << 1;
            n++;
        }
        if ( n == 1 ) {
            index++;
            goto loop;
        }
    }
        if ( n == 1 ) {
            byte0 = string->buffer[index];
            value = byte0;
            *offset = 0;
        }

        if ( n == 2 ) {
            byte0 = string->buffer[index];
            byte0 = byte0 & 0x3F;
            byte0 = byte0 << 6;
            byte1 = string->buffer[index+1];
            byte1 = byte1 & 0x7F;
            value = byte0 | byte1;
            *offset = 1;
        }
        if ( n == 3 ) {
            byte0 = string->buffer[index];
            byte0 = byte0 & 0x1F;
            byte0 = byte0 << 12;
            byte1 = string->buffer[index+1];
            byte1 = byte1 & 0x7F;
            byte1 = byte1 << 6;
            byte2 = string->buffer[index+2];
            byte2 = byte2 & 0x7F;
            value = byte0 | byte1 | byte2;
            *offset = 2;
        }
        if ( n == 4 ) {
            byte0 = string->buffer[index];
            byte0 = byte0 & 0xF;
            byte0 = byte0 << 18;
            byte1 = string->buffer[index+1];
            byte1 = byte1 & 0x7F;
            byte1 = byte1 << 12;
            byte2 = string->buffer[index+2];
            byte2 = byte2 & 0x7F;
            byte2 = byte2 << 6;
            byte3 = string->buffer[index+3];
            byte3 = byte3 & 0x7F;
            value = byte0 | byte1 | byte2 | byte3;
            *offset = 3;
        }
    return value;
}

vortex_byte VortexString_GetByte(VortexString string, vortex_ulong index) {
    if ( index > string->size_in_bytes ) return 0;
    if ( index < 0 ) return 0;
    return string->buffer[index];
}

void VortexString_SetByte(VortexString string, vortex_ulong index, vortex_byte byte) {
    if ( index > string->size_in_bytes ) return;
    if ( index < 0 ) return;
    string->buffer[index] = byte;
}

VortexString VortexString_AppendString(VortexString BaseString, VortexString AppendingString) {
    VortexString string = VortexString_AddStrings(BaseString, AppendingString);
    VortexString_Destroy(BaseString);
    return string;
}

VortexString VortexString_Copy(VortexString string) {
    if ( string == NULL ) return NULL;
    return VortexString_NewFromCString(VortexString_GetBuffer(string));
}

vortex_int VortexString_AreStringsEqual(VortexString a, VortexString b) {
    return ( strcmp(VortexString_GetBuffer(a), VortexString_GetBuffer(b)) == 0 );
}

char* VortexString_ConvertToCString(VortexString string) {
    if ( string == NULL ) return NULL;
    char* str = string->buffer;
    free(string);
    return str ;
}

void VortexString_Print(VortexString string) {
    printf("%s", string->buffer);
}

vortex_int* VortexString_GetUTF32String(VortexString string, vortex_ulong* length) {
    vortex_ulong i = 0;
    vortex_ulong j = 0;
    vortex_ulong offset = 0 ;
    vortex_ulong size_of_string = VortexString_GetSize(string);
    vortex_int* utf32string = vortex_c_array(size_of_string+1, vortex_int);
    while ( i < size_of_string ) {
        utf32string[j] = VortexString_GetCharacter(string, i, &offset);
        i += offset;
        j++;
        i++;
    }
    utf32string[size_of_string] = '\0';
    *length = VortexString_GetLength(string);
    return utf32string;
}

//For systems and environments that still don't support unicode
//mangles unicode characters into utf_<<bytes of the unicode character>>

VortexString VortexString_GetStringForASCII(VortexString string) {
    vortex_ulong i = 0;
    vortex_ulong j = 0;
    vortex_ulong x = 0;
    vortex_ulong n = 0;
    vortex_ulong str_size = 1;
    char string_num[100];
    vortex_sbyte byte = 0;
    vortex_byte ubyte = 0;
    VortexString ascii_string = NULL;
    char* str = vortex_c_array(1, char);
    str[0] = '\0';
    string_num[0] = '0';
    string_num[1] = '0';
    string_num[2] = '0';
    string_num[3] = '\0';
    while ( i < string->size_in_bytes  ) {
        if ( string->buffer[i] < 0 ) {
            str = vortex_realloc(str, str_size+4, str_size, char, 1);
            str_size += 4;
            str[str_size-5] = 'u';
            str[str_size-4] = 't';
            str[str_size-3] = 'f';
            str[str_size-2] = '_';
            str[str_size-1] = '\0';
            byte = string->buffer[i];
            n = 0;
            while (byte < 0) {
                byte = byte << 1;
                n++;
            }
            x = 0;
            ubyte = byte = string->buffer[i];
            while ( x < n ) {
                snprintf(string_num, sizeof(string_num), "%u", ubyte);
                j = 0;
                while ( j < strlen(string_num) ) {
                 str = vortex_realloc(str, str_size+1, str_size, char, 1);
                 str_size++;
                 str[str_size-2] = string_num[j];
                 str[str_size-1] = '\0';
                 j++;
                }
                i++;
                if (  i >= string->size_in_bytes ) {
                    break;
                }
                ubyte = byte = string->buffer[i];
                x++;
            }
            i--;
        } else {
            str = vortex_realloc(str, str_size+1, str_size, char, 1);
            str_size++;
            str[str_size-2] = string->buffer[i];
            str[str_size-1] = '\0';
        }
        i++;
    }
    ascii_string = VortexString_NewFromCString(str);
    free(str);
    return ascii_string;
}

static vortex_int Vortex_GetEscapeCharacter(vortex_int c) {
    switch (c) {
        case '\a':
            return 'a';
            break;
        case '\b':
            return 'b';
            break;
        case '\f':
            return 'f';
            break;
        case '\n':
            return 'n';
            break;
        case '\r':
            return 'r';
            break;
        case '\t':
            return 't';
            break;
        case '\v':
            return 'v';
            break;
        default:
            break;
    }
    return 'n';
}

VortexString VortexString_SwapEscapeSequencesWithCharacters(VortexString string) {
    vortex_ulong i = 0;
    vortex_ulong j = 0;
    vortex_ulong x = 0;
    vortex_int c = 0;
    vortex_ulong str_size = 0;
    vortex_ulong str2_size = 0;
    VortexString retstring = NULL ;
    int* str = VortexString_GetUTF32String(string, &str_size);
    int* str2 = NULL;
    while ( i < str_size ) {
        c = str[i];
        if ( c == '\a' || c == '\b' || c == '\f' || c == '\n'
            || c == '\r' || c == '\t' || c == '\v' ) {
            j = 0;
            x = 0;
            str2_size = str_size+2;
            str2 = vortex_c_array(str2_size, vortex_int);
            str2[str2_size-1] = '\0';
            while ( j < str2_size ) {
                if ( j != i && j != (i+1) && j != (str2_size-1) ) {
                    str2[j] = str[j+x];
                }
                if ( j == i ) {
                    str2[j] = '\\';
                }
                if ( j == i+1 ) {
                    str2[j] = Vortex_GetEscapeCharacter(c);
                    x = -1;
                }
                if ( j == (str2_size-1) ) {
                    free(str);
                    str = str2;
                    str_size = str2_size;
                    i++;
                }
                j++;
            }
        }
        i++;
    }
    VortexString_Destroy(string);
    retstring = VortexString_NewFromUTF32(str, str_size);
    free(str);
    return retstring;
}

VortexAny VortexAny_New(VortexAny any, vortex_ulong size) {
    vortex_byte* ptr = malloc(size);
    memcpy(ptr, any, size);
    return ptr;
}

VortexStack VortexStack_New(void) {
    VortexStack stack = vortex_new_mem_of_type(struct VortexStack_s);
    stack->list = VortexList_New();
    return stack;
}

void VortexStack_Destroy(VortexStack stack) {
    if (stack != NULL) VortexList_Destroy(stack->list);
    free(stack);
}

void VortexStack_Push(VortexStack stack, VortexAny data) {
    VortexList_AddToList(stack->list, data);
}

VortexAny VartexStack_Pop(VortexStack stack) {
    if ( VortexStack_IsEmpty(stack) ) return NULL;
    VortexAny data = VortexList_GetData(VortexList_GetLastNode(stack->list));
    VortexList_DestroyNode(stack->list, VortexList_GetLastNode(stack->list));
    return data;
}

VortexAny VortexStack_Peek(VortexStack stack) {
    return VortexList_GetData(VortexList_GetLastNode(stack->list));
}

vortex_int VortexStack_IsEmpty(VortexStack stack) {
    return (VortexList_GetNumOfNodes(stack->list) == 0) ? 1 : 0;
}

VortexList VortexStack_GetList(VortexStack stack) {
    return stack->list;
}

VortexListNode VortexStack_GetListNode(VortexStack stack) {
    return VortexList_GetLastNode(stack->list);
}
