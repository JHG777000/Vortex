/*
 Copyright (c) 2014-2019 Jacob Gordon. All rights reserved.

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

//File: RKMem.c
//RKMem, provides some nice macros and a funtion to make dealing with memory a little easier in C.
//As well as a double linked-list implementation, RKList.
//And a dictionary type, RKStore.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <RKLib/RKMem.h>

 struct RKArray_s { void** items ; RKULong num_of_items ; size_t size_of_type ; } ;

 struct RKStack_s { RKList list ; } ;

 struct RKString_s { RKLong size_in_characters ; RKULong size_in_bytes ; char* string ; } ;

 struct RKIndex_s { RKStore store ; int max_num_of_items ; int num_of_items ; } ;

 typedef struct RKStore_letter_s { RKList_node node ; struct RKStore_letter_s* next_alphabet ; } RKStore_letter ;

 struct RKStore_s { RKStore_letter* dictionary ; RKList items ; } ;

 struct RKList_node_s { struct RKList_node_s* before ; struct RKList_node_s* after ; void* data ; RKString string ; } ;

 struct RKList_s { int num_of_nodes ; RKList_node first ; RKList_node last ; } ;

 void* RKMem_Realloc_Func( void* data, size_t newsize, size_t oldsize, int NULLonError0No1Yes ) {

    void* newdata ;

    newdata = malloc(newsize) ;

    if ( newdata != NULL ) {

        memcpy(newdata, data, oldsize) ;

        free(data) ;

        return newdata ;

    } else {

        if ( NULLonError0No1Yes ) {

            free(data) ;

            return NULL ;

        } else {

            return data ;
        }
    }
}

RKArray RKArray_NewArray( void ) {

  RKArray array = RKMem_NewMemOfType(struct RKArray_s) ;

  array->num_of_items = 0 ;

  array->items = NULL ;

  return array ;
}

RKArray RKArray_NewArrayWithBaseSize( int base_size ) {

  RKArray array = RKMem_NewMemOfType(struct RKArray_s) ;

  array->items = RKMem_CArray(base_size, void*) ;

  array->num_of_items = base_size ;

  return array ;
}

RKArray RKArray_NewArrayFromBuffer( void* buffer[], int size ) {

  RKArray array = RKArray_NewArrayWithBaseSize(size) ;

  memcpy(array->items,buffer,array->num_of_items) ;

  return array ;
}

void RKArray_DestroyArray( RKArray array ) {

  free(array->items) ;

  free(array) ;
}

void RKArray_AddItem( RKArray array, void* item ) {

  array->num_of_items++ ;

  if ( array->items == NULL ) {

   array->items = RKMem_CArray(array->num_of_items, void*) ;

  } else {

   array->items = RKMem_Realloc(array->items,
    array->num_of_items,
    array->num_of_items-1,
    void*,
    1) ;

 }

 array->items[array->num_of_items-1] = item ;

}

void RKArray_AddArray( RKArray array, void* items[], int num_of_items_to_add ) {

  RKArray_AddSpace(array,num_of_items_to_add) ;

  memcpy(array->items+(array->num_of_items-num_of_items_to_add),items,array->num_of_items) ;

}

void RKArray_AddSpace( RKArray array, int space_to_add ) {

  if ( array->items == NULL ) {

   array->num_of_items = space_to_add ;

   array->items = RKMem_CArray(array->num_of_items, void*) ;

  } else {

   array->num_of_items += space_to_add ;

   array->items = RKMem_Realloc(array->items,
    array->num_of_items,
    array->num_of_items-space_to_add,
    void*,
    1) ;

 }

}

void* RKArray_GetItem( RKArray array, int index ) {

  if ( index >= 0 && index < array->num_of_items ) {

    return array->items[index] ;
  }

  return NULL ;
}

int RKArray_SetItem( RKArray array, int index, void* item ) {

  if ( index >= 0 && index < array->num_of_items ) {

    array->items[index] = item ;

    return 1 ;
  }

  return 0 ;
}

RKList RKList_NewList( void ) {

    RKList newlist = RKMem_NewMemOfType(struct RKList_s) ;

    newlist->num_of_nodes = 0 ;

    newlist->first = NULL ;

    newlist->last = NULL ;

    return newlist ;
}

RKList_node RKList_NewNode(  RKList_node before, RKList_node after, void* data ) {

    RKList_node node = RKMem_NewMemOfType(struct RKList_node_s) ;

    node->before = before ;

    node->after = after ;

    node->data = data ;

    node->string = NULL ;

    return node ;
}

RKList RKList_NewListFromArray( void* array, RKList_GetDataFromArrayFuncType GetDataFromArrayFunc, int size ) {

    RKList newlist = RKList_NewList() ;

    RKList_CopyToListFromArray(newlist, array, GetDataFromArrayFunc, size) ;

    return newlist ;
}

RKList_node RKList_AddToList( RKList list, void* data ) {

    if ( list->num_of_nodes == 0 ) {

        list->first = RKMem_NewMemOfType(struct RKList_node_s) ;

        list->first->before = NULL ;

        list->first->after = NULL ;

        list->last = list->first ;

    } else {

        list->last->after = RKMem_NewMemOfType(struct RKList_node_s) ;

        list->last->after->before = list->last ;

        list->last->after->after = NULL ;

        list->last = list->last->after ;
    }

    list->last->data = data ;

    list->last->string = NULL ;

    list->num_of_nodes++ ;

    return list->last ;
}

RKList_node RKList_AddNodeToList( RKList_node node, RKList list ) {

    return RKList_AddToList(list, node->data) ;
}

void RKList_NodeSwap( RKList list, RKList_node node_a, RKList_node node_b ) {

    RKList_node before = (node_a->before == node_b) ? node_a : node_a->before ;

    RKList_node after = (node_a->after == node_b) ? node_a : node_a->after ;

    node_a->before = (node_b->before == node_a) ? node_b : node_b->before ;

    node_a->after = (node_b->after == node_a) ? node_b : node_b->after ;

    if ( node_a->before != NULL ) node_a->before->after = node_a ;

    if ( node_a->after != NULL ) node_a->after->before = node_a ;

    if ( node_a->before == NULL ) list->first = node_a ;

    if ( node_a->after == NULL ) list->last = node_a ;

    node_b->before = before ;

    node_b->after = after ;

    if ( node_b->before != NULL ) node_b->before->after = node_b ;

    if ( node_b->after != NULL ) node_b->after->before = node_b ;

    if ( node_b->before == NULL ) list->first = node_b ;

    if ( node_b->after == NULL ) list->last = node_b ;

}

void RKList_DataSwap( RKList_node node_a, RKList_node node_b ) {

    void* data = node_a->data ;

    node_a->data = node_b->data ;

    node_b->data = data ;
}

void RKList_InsertNodeAToNodeB( RKList list, RKList_node node_a, RKList_node node_b ) {

    if ( node_a->before != NULL ) node_a->before->after = node_a->after ;

    if ( node_a->after != NULL ) node_a->after->before = node_a->before ;

    if ( node_a->before == NULL ) list->first = node_a->after ;

    if ( node_a->after == NULL ) list->last = node_a->before ;

    if ( node_b->before == NULL ) list->first = node_a ;

    if ( node_b->after == NULL ) list->last = node_a ;

    node_a->before = node_b->before ;

    node_a->after = node_b ;

    if ( node_b->before != NULL ) node_b->before->after = node_a ;

    node_b->before = node_a ;
}

RKList_node RKList_MoveNodeFromListToList( RKList_node node, RKList list_a, RKList list_b ) {

    RKList_node new_node = RKList_AddNodeToList( node, list_b ) ;

    RKList_DeleteNode(list_a, node) ;

    return new_node ;
}

void RKList_CopyList(RKList list_a, RKList list_b) {

    RKList_node node = RKList_GetFirstNode(list_a) ;

    while ( node != NULL ) {

        RKList_AddNodeToList( node, list_b ) ;

        node = RKList_GetNextNode(node) ;
    }
}

void RKList_CopyToListFromArray(RKList list, void* array, RKList_GetDataFromArrayFuncType GetDataFromArrayFunc, int size) {

    void* data = NULL ;

    int i = 0 ;

    while ( i < size ) {

        data = GetDataFromArrayFunc(array,i) ;

        RKList_AddToList(list, data) ;

        i++ ;
    }

}

void RKList_SetData(RKList_node node, void* data) {

    node->data = data ;
}

void* RKList_GetData(RKList_node node) {

    if ( node == NULL ) return NULL ;

    return node->data ;
}

static void RKList_SetString(RKList_node node, RKString string) {

    node->string = string ;
}

static RKString RKList_GetString(RKList_node node) {

    if ( node == NULL ) return NULL ;

    return node->string ;
}

RKList_node RKList_GetNextNode(RKList_node node) {

    if ( node == NULL ) return NULL ;

    return node->after ;
}

RKList_node RKList_GetPreviousNode(RKList_node node) {

    if ( node == NULL ) return NULL ;

    return node->before ;
}

RKList_node RKList_GetFirstNode(RKList list) {

    if ( list == NULL ) return NULL ;

    return list->first ;
}

RKList_node RKList_GetLastNode(RKList list) {

    if ( list == NULL ) return NULL ;

    return list->last ;
}

int RKList_GetNumOfNodes(RKList list) {

    if ( list == NULL ) return -1 ;

    return list->num_of_nodes ;
}

void RKList_DeleteNode( RKList list, RKList_node node ) {

    if ( node == NULL ) return ;

    if ( ( node->before == NULL ) || ( node->after == NULL ) ) {

     if ( ( node->before == NULL ) && ( node->after == NULL ) ) {

         list->first = NULL ;

         list->last = NULL ;
     }

     if ( ( node->before == NULL ) && ( node->after != NULL ) ) {

         list->first = node->after ;

         list->first->before = NULL ;
     }

     if ( ( node->before != NULL ) && ( node->after == NULL ) ) {

         list->last = node->before ;

         list->last->after = NULL ;
     }

    }

    if ( ( node->before != NULL ) && ( node->after != NULL ) ) {

        node->before->after = node->after ;

        node->after->before = node->before ;
    }

    list->num_of_nodes-- ;

    if ( node->string != NULL ) RKString_DestroyString(node->string) ;

    free(node) ;
}

RKList_node RKList_DeleteNodeAndReturnNextNode( RKList list, RKList_node node ) {

    RKList_node next_node = RKList_GetNextNode(node) ;

    RKList_DeleteNode(list, node) ;

    return next_node ;
}

void RKList_DeleteList( RKList list ) {

    while ( list->first != NULL ) {

        RKList_DeleteNode(list,list->first) ;
    }

    free(list) ;
}

void RKList_DeleteAllNodesInList( RKList list ) {

    while ( list->first != NULL ) {

        RKList_DeleteNode(list,list->first) ;
    }
}

RKList_node RKList_GetNode( RKList list, int index ) {

    int i = 1 ;

    RKList_node node = list->first ;

    while ( i < index ) {

        if ( node == NULL ) return NULL ;

        node = node->after ;

        i++ ;
    }

    return node ;
}

int RKList_GetIndex( RKList list, RKList_node node ) {

    int i = 0 ;

    RKList_node node2 = list->first ;

    while ( node2 != NULL ) {

        if ( node == node2 ) break ;

        node2 = node2->after ;

        i++ ;
    }

    return i ;
}

RKList_node RKList_GetNextNodeAfterN( RKList_node node, int n ) {

    int i = 0 ;

    while ( i < n ) {

        if ( node == NULL ) return NULL ;

        node = node->after ;

        i++ ;
    }

    return node ;
}

RKList_node RKList_GetPreviousNodeAfterN( RKList_node node, int n ) {

    int i = 0 ;

    while ( i < n ) {

        if ( node == NULL ) return NULL ;

        node = node->before ;

        i++ ;
    }

    return node ;
}

void RKList_DeleteNodeWithIndex( RKList list, int index ) {

    RKList_node node = RKList_GetNode(list, index) ;

    RKList_DeleteNode(list, node) ;
}

void RKList_IterateListWith( RKMemIteratorFuncType iterator, RKList list ) {

    RKList_node node = list->first ;

    void* data = NULL ;

    while ( node != NULL ) {

        data = RKList_GetData(node) ;

        iterator(data) ;

        node = RKList_GetNextNode(node) ;
    }
}

static RKStore_letter* RKS_NewBinaryNode( void ) {

    RKStore_letter* rks_alphabet = NULL ;

    int size = 2 ;

    rks_alphabet = RKMem_CArray(size, RKStore_letter) ;

    int i = 0 ;

    while ( i < size ) {

        rks_alphabet[i].node = NULL ;

        rks_alphabet[i].next_alphabet = NULL ;

        i++ ;
    }

    return rks_alphabet ;

}

static RKList_node RKS_GetSetNode( RKStore store, const char* label, RKList_node node, int flag, int find ) {

    int i = 0 ;

    int j = 0 ;

    int value = 0 ;

    signed char byte = 0 ;

    int size = (int) strlen( label ) ;

    int size2 = sizeof(char) * CHAR_BIT ;

    RKList_node retnode = NULL ;

    RKStore_letter* current_alphabet = NULL ;

    if ( store->dictionary == NULL ) {

        store->dictionary = RKS_NewBinaryNode() ;
    }

    current_alphabet = store->dictionary ;

    while ( i < size ) {

        byte = label[i] ;

        j = 0 ;

        while ( j < size2 ) {

            value = (byte < 0) ? 1 : 0 ;

            byte = byte << 1 ;

            if ( i == ( size - 1 ) && j == ( size2 - 1 ) ) {

                if ( node == NULL ) {

                    if (!flag) {

                        return current_alphabet[value].node ;

                    } else {

                        retnode = current_alphabet[value].node ;

                        RKString_DestroyString(RKList_GetString(current_alphabet[value].node)) ;

                        RKList_SetString(current_alphabet[value].node, NULL) ;

                        current_alphabet[value].node = NULL ;

                        return retnode ;
                    }

                } else {

                    current_alphabet[value].node = node ;

                    RKList_SetString(current_alphabet[value].node, RKString_NewStringFromCString(label)) ;

                    return current_alphabet[value].node ;
                }

            } else {

                if ( !find ) if ( current_alphabet[value].next_alphabet == NULL ) current_alphabet[value].next_alphabet = RKS_NewBinaryNode() ;

                if ( find ) if ( current_alphabet[value].next_alphabet == NULL ) return NULL ;

                current_alphabet = current_alphabet[value].next_alphabet ;

            }

            j++ ;
        }

        i++ ;
    }

    return NULL ;
}

RKStore RKStore_NewStore( void ) {

    RKStore store = RKMem_NewMemOfType(struct RKStore_s) ;

    store->dictionary = NULL ;

    store->items = NULL ;

    return store ;
}

static RKList_node RKS_AddItem( RKStore store, void* item ) {

    if ( store->items == NULL ) store->items = RKList_NewList() ;

    return RKList_AddToList(store->items, item) ;
}

static int RKS_StoreItem( RKStore store, void* item, const char* label ) {

    RKList_node node = RKS_GetSetNode(store, label, NULL, 0, 1) ;

    if ( node == NULL ) return 0 ;

    RKList_SetData(node, item) ;

    return 1 ;
}

int RKStore_AddItem( RKStore store, void* item, const char* label ) {

    if ( !(RKStore_ItemExists(store, label)) ) {

        RKList_node node = RKS_AddItem(store, item) ;

        node = RKS_GetSetNode(store, label, node, 0, 0) ;

        if ( node == NULL ) return 0 ;

    } else {

        return RKS_StoreItem(store, item, label) ;
    }

    return 0 ;
}

int RKStore_RemoveItem( RKStore store, const char* label ) {

    RKList_node node = RKS_GetSetNode(store, label, NULL, 1, 1) ;

    if ( node == NULL ) return 0 ;

    RKList_DeleteNode(store->items, node) ;

    return 1 ;
}

void* RKStore_GetItem( RKStore store, const char* label ) {

    RKList_node node = RKS_GetSetNode(store, label, NULL, 0, 1) ;

    if ( node == NULL ) return NULL ;

    return RKList_GetData(node) ;
}

int RKStore_ItemExists( RKStore Store, const char* label ) {

    RKList_node node = RKS_GetSetNode(Store, label, NULL, 0, 1) ;

    if ( node != NULL ) return 1 ;

    return 0 ;
}

int RKStore_GetNumOfItems( RKStore Store ) {

    return RKList_GetNumOfNodes(RKStore_GetList(Store)) ;
}

int RKStore_AddItemToList( RKStore store, void *item ) {

    if ( RKS_AddItem(store, item) == NULL ) return 0 ;

    return 1 ;
}

RKList RKStore_GetList( RKStore store ) {

    if ( store->items == NULL ) store->items = RKList_NewList() ;

    return store->items ;
}

RKString RKStore_GetStoreLabelFromListNode( RKList_node node ) {

    if ( node == NULL ) return NULL ;

    return RKList_GetString(node) ;
}

void RKStore_IterateStoreWith( RKMemIteratorFuncType iterator, RKStore store ) {

    if (store->items != NULL) RKList_IterateListWith(iterator, store->items) ;
}

int RKStore_IsStoreEmpty( RKStore store ) {

    return (RKList_GetNumOfNodes(store->items) == 0) ? 1 : 0 ;
}

static void RKS_DestroyBinaryNodes( RKStore_letter* alphabet ) {

    int i = 0 ;

    while ( i < 2 ) {

        if (alphabet[i].next_alphabet != NULL)  {

            RKS_DestroyBinaryNodes(alphabet[i].next_alphabet) ;

            alphabet[i].next_alphabet = NULL ;
        }

        i++ ;
    }

    free(alphabet) ;
}

void RKStore_DestroyStore( RKStore store ) {

    if (store->dictionary != NULL) RKS_DestroyBinaryNodes(store->dictionary) ;

    if (store->items != NULL) RKList_DeleteList(store->items) ;

    free(store) ;
}

RKIndex RKIndex_NewIndex( int max_num_of_items ) {

    RKIndex rkindex = RKMem_NewMemOfType(struct RKIndex_s) ;

    rkindex->max_num_of_items = max_num_of_items ;

    rkindex->store = RKStore_NewStore() ;

    rkindex->num_of_items = 0 ;

    return rkindex ;
}

void RKIndex_DestroyIndex( RKIndex rkindex ) {

    RKStore_DestroyStore(rkindex->store) ;

    free(rkindex) ;
}

int RKIndex_AddItem( RKIndex rkindex, void *item ) {

    char string[100] ;

    int index = rkindex->num_of_items ;

    if ( rkindex->max_num_of_items > 0 ) {

        if ( index > rkindex->max_num_of_items ) return 0 ;
    }

    if ( index < 0 ) return 0 ;

    snprintf(string, 100, "%d", index) ;

    if ( RKStore_ItemExists(rkindex->store, string) ) return 0 ;

    rkindex->num_of_items++ ;

    return RKStore_AddItem(rkindex->store, item, string) ;
}

int RKIndex_SetItem( RKIndex rkindex, void *item, int index ) {

    char string[100] ;

    if ( rkindex->max_num_of_items > 0 ) {

        if ( index > rkindex->max_num_of_items ) return 0 ;
    }

    if ( index < 0 ) return 0 ;

    snprintf(string, 100, "%d", index) ;

    if ( !RKStore_ItemExists(rkindex->store, string) ) return 0 ;

    return RKStore_AddItem(rkindex->store, item, string) ;
}

void* RKIndex_GetItem( RKIndex rkindex, int index ) {

    char string[100] ;

    if ( rkindex->max_num_of_items > 0 ) {

        if ( index > rkindex->max_num_of_items ) return NULL ;
    }

    if ( index < 0 ) return NULL ;

    snprintf(string, 100, "%d", index) ;

    return RKStore_GetItem(rkindex->store, string) ;
}

int RKIndex_RemoveItem( RKIndex rkindex, int index ) {

    char string[100] ;

    if ( rkindex->max_num_of_items > 0 ) {

        if ( index > rkindex->max_num_of_items ) return 0 ;
    }

    if ( index < 0 ) return 0 ;

    rkindex->num_of_items-- ;

    snprintf(string, 100, "%d", index) ;

    return RKStore_RemoveItem(rkindex->store, string) ;
}

int RKIndex_GetMaxNumOfItems( RKIndex rkindex ) {

    return rkindex->max_num_of_items ;
}

int RKIndex_GetNumOfItems( RKIndex rkindex ) {

    return rkindex->num_of_items ;
}

int RKIndex_IsIndexEmpty( RKIndex rkindex ) {

    return RKStore_IsStoreEmpty(rkindex->store) ;
}

RKString RKString_NewEmptyString( size_t size_in_bytes ) {

    RKString string = RKMem_NewMemOfType(struct RKString_s) ;

    string->size_in_bytes = size_in_bytes ;

    string->string = RKMem_CArray(string->size_in_bytes, char) ;

    string->string[string->size_in_bytes-1] = '\0' ;

    string->size_in_bytes -= 1 ; //not counting '\0'

    string->size_in_characters = -1 ;

    return string ;
}

RKString RKString_NewStringFromUTF32( const int* text, int num_of_characters ) {

    int i = 0 ;

    int character = 0 ;

    unsigned int value = 0 ;

    unsigned int value0 = 0 ;

    unsigned int value1 = 0 ;

    unsigned int value2 = 0 ;

    RKString string = NULL ;

    int utf8string_size = 1 ;

    char* utf8string = RKMem_CArray(1, char) ;

    utf8string[utf8string_size-1] = '\0' ;

    while ( i < num_of_characters ) {

        character = text[i] ;

        if ( character >= 0 && character <= 0x007F ) {

            utf8string = RKMem_Realloc(utf8string, utf8string_size+1, utf8string_size, char, 1) ;

            utf8string_size++ ;

            utf8string[utf8string_size-2] = character ;

            utf8string[utf8string_size-1] = '\0' ;
        }

        if ( character >= 0x0080 && character <= 0x07FF ) {

            value = character ;

            value = value >> 6 ;

            value = value | 0xC0 ;

            value0 = character ;

            value0 = value0 & 0x3F ;

            value0 = value0 | 0x80 ;

            utf8string = RKMem_Realloc(utf8string, utf8string_size+2, utf8string_size, char, 1) ;

            utf8string_size+=2 ;

            utf8string[utf8string_size-3] = value ;

            utf8string[utf8string_size-2] = value0 ;

            utf8string[utf8string_size-1] = '\0' ;
        }

        if ( character >= 0x0800 && character <= 0xFFFF ) {

            value = character ;

            value = value >> 12 ;

            value = value | 0xE0 ;

            value0 = character ;

            value0 = value0 >> 6 ;

            value0 = value0 & 0xBF ;

            value0 = value0 | 0x80 ;

            value1 = character ;

            value1 = value1 & 0x3F ;

            value1 = value1 | 0x80 ;

            utf8string = RKMem_Realloc(utf8string, utf8string_size+3, utf8string_size, char, 1) ;

            utf8string_size+=3 ;

            utf8string[utf8string_size-4] = value ;

            utf8string[utf8string_size-3] = value0 ;

            utf8string[utf8string_size-2] = value1 ;

            utf8string[utf8string_size-1] = '\0' ;

        }

        if ( character >= 0x10000 && character <= 0x10FFFF ) {

            value = character ;

            value = value >> 18 ;

            value = value | 0xF0 ;

            value0 = character ;

            value0 = value0 >> 12 ;

            value0 = value0 & 0xBF ;

            value0 = value0 | 0x80 ;

            value1 = character ;

            value1 = value1 >> 6 ;

            value1 = value1 & 0xBF ;

            value1 = value1 | 0x80 ;

            value2 = character ;

            value2 = value2 & 0x3F ;

            value2 = value2 | 0x80 ;

            utf8string = RKMem_Realloc(utf8string, utf8string_size+4, utf8string_size, char, 1) ;

            utf8string_size+=4 ;

            utf8string[utf8string_size-5] = value ;

            utf8string[utf8string_size-4] = value0 ;

            utf8string[utf8string_size-3] = value1 ;

            utf8string[utf8string_size-2] = value2 ;

            utf8string[utf8string_size-1] = '\0' ;
        }

        i++ ;
    }

    string = RKString_NewStringFromCString(utf8string) ;

    free(utf8string) ;

    return string ;
}

RKString RKString_NewStringFromBuffer( const char* text, size_t size_in_bytes ) {

    RKString string = RKMem_NewMemOfType(struct RKString_s) ;

    string->size_in_bytes = size_in_bytes ;

    string->string = RKMem_CArray(string->size_in_bytes, char) ;

    string->string[string->size_in_bytes-1] = '\0' ;

    int i = 0 ;

    while ( i < string->size_in_bytes-1 ) {

        string->string[i] = text[i] ;

        i++ ;
    }

    string->size_in_bytes -= 1 ; //not counting '\0'

    string->size_in_characters = -1 ;

    return string ;
}

RKString RKString_NewStringFromCString( const char* text ) {

    RKString string = RKMem_NewMemOfType(struct RKString_s) ;

    string->size_in_bytes = strlen(text) ;

    string->string = RKMem_CArray(string->size_in_bytes+1, char) ;

    strcpy(string->string, text) ;

    string->size_in_characters = -1 ;

    return string ;
}

RKString RKString_NewStringFromTwoStrings( RKString a, RKString b ) {

    RKString string = RKMem_NewMemOfType(struct RKString_s) ;

    string->size_in_bytes = a->size_in_bytes + b->size_in_bytes ;

    string->string = RKMem_CArray(string->size_in_bytes+1, char) ;

    int i = 0 ;

    int j = 0 ;

    while ( j < a->size_in_bytes ) {

        string->string[i] = a->string[j] ;

        i++ ;

        j++ ;
    }

    j = 0 ;

    while ( j < b->size_in_bytes ) {

        string->string[i] = b->string[j] ;

        i++ ;

        j++ ;
    }

    string->string[i] = '\0' ;

    string->size_in_characters = -1 ;

    return string ;
}

void RKString_DestroyString( RKString string ) {

    if ( string == NULL ) return ;

    free(string->string) ;

    free(string) ;
}

RKULong RKString_GetSize( RKString string ) {

    return string->size_in_bytes ;
}

RKULong RKString_GetSizeInBytes( RKString string ) {

    return string->size_in_bytes+1 ;
}

RKULong RKString_GetLength( RKString string ) {

    int i = 0 ;

    int n = 0 ;

    signed char byte = 0 ;

    int num_of_characters = 0 ;

    if ( string->size_in_characters == -1 ) {

        while ( i < string->size_in_bytes ) {

            byte = string->string[i] ;

            if ( byte >= 0 ) {

                if ( byte > 0 ) num_of_characters++ ;

            } else {

                n = 0 ;

                while (byte < 0) {

                    byte = byte << 1 ;

                    n++ ;
                }

                i += (n-1) ;

                num_of_characters++ ;
            }

            i++ ;
        }

        string->size_in_characters = num_of_characters ;
    }

    return string->size_in_characters ;
}

char* RKString_GetString( RKString string ) {

    if ( string == NULL ) return NULL ;

    return string->string ;
}

int RKString_GetCharacter( RKString string, int index, int* offset ) {

    int n = 0 ;

    signed char byte = 0 ;

    unsigned int byte0 = 0 ;

    unsigned int byte1 = 0 ;

    unsigned int byte2 = 0 ;

    unsigned int byte3 = 0 ;

    unsigned int value = 0 ;
loop:
    if ( index > string->size_in_bytes ) return 0 ;

    if ( index < 0 ) return 0 ;

    byte = string->string[index] ;

    if ( byte > 0 ) {

        n = 1 ;

    } else {

        n = 0 ;

        while (byte < 0) {

            byte = byte << 1 ;

            n++ ;
        }

        if ( n == 1 ) {

            index++ ;

            goto loop ;
        }

    }
        if ( n == 1 ) {

            byte0 = string->string[index] ;

            value = byte0 ;

            *offset = 0 ;
        }

        if ( n == 2 ) {

            byte0 = string->string[index] ;

            byte0 = byte0 & 0x3F ;

            byte0 = byte0 << 6 ;

            byte1 = string->string[index+1] ;

            byte1 = byte1 & 0x7F ;

            value = byte0 | byte1 ;

            *offset = 1 ;
        }

        if ( n == 3 ) {

            byte0 = string->string[index] ;

            byte0 = byte0 & 0x1F ;

            byte0 = byte0 << 12 ;

            byte1 = string->string[index+1] ;

            byte1 = byte1 & 0x7F ;

            byte1 = byte1 << 6 ;

            byte2 = string->string[index+2] ;

            byte2 = byte2 & 0x7F ;

            value = byte0 | byte1 | byte2 ;

            *offset = 2 ;

        }

        if ( n == 4 ) {

            byte0 = string->string[index] ;

            byte0 = byte0 & 0xF ;

            byte0 = byte0 << 18 ;

            byte1 = string->string[index+1] ;

            byte1 = byte1 & 0x7F ;

            byte1 = byte1 << 12 ;

            byte2 = string->string[index+2] ;

            byte2 = byte2 & 0x7F ;

            byte2 = byte2 << 6 ;

            byte3 = string->string[index+3] ;

            byte3 = byte3 & 0x7F ;

            value = byte0 | byte1 | byte2 | byte3 ;

            *offset = 3 ;

        }

    return value ;
}

char RKString_GetByte( RKString string, int index ) {

    if ( index > string->size_in_bytes ) return 0 ;

    if ( index < 0 ) return 0 ;

    return string->string[index] ;
}

void RKString_SetByte( RKString string, int index, char byte ) {

    if ( index > string->size_in_bytes ) return ;

    if ( index < 0 ) return ;

    string->string[index] = byte ;
}

RKString RKString_AppendString( RKString BaseString, RKString AppendingString ) {

    RKString string = RKString_AddStrings(BaseString, AppendingString) ;

    RKString_DestroyString(BaseString) ;

    return string ;
}

RKString RKString_CopyString( RKString string ) {

    if ( string == NULL ) return NULL ;

    return RKString_NewStringFromCString(RKString_GetString(string)) ;
}

int RKString_AreStringsEqual( RKString a, RKString b ) {

    return ( strcmp(RKString_GetString(a), RKString_GetString(b)) == 0 ) ;
}

char* RKString_ConvertToCString( RKString string ) {

    if ( string == NULL ) return NULL ;

    char* str = string->string ;

    free(string) ;

    return str ;
}

void RKString_PrintString( RKString string ) {

    printf("%s", string->string) ;
}

int* RKString_GetUTF32String( RKString string, RKULong* length ) {

    int i = 0 ;

    int j = 0 ;

    int offset = 0 ;

    RKULong size_of_string = RKString_GetSize(string) ;

    int* utf32string = RKMem_CArray(size_of_string+1, int) ;

    while ( i < size_of_string ) {

        utf32string[j] = RKString_GetCharacter(string, i, &offset) ;

        i += offset ;

        j++ ;

        i++ ;
    }

    utf32string[size_of_string] = '\0' ;

    *length = RKString_GetLength(string) ;

    return utf32string ;
}

//For systems and environments that still don't support unicode
//mangles unicode characters into utf_<<bytes of the unicode character>>

RKString RKString_GetStringForASCII( RKString string ) {

    int i = 0 ;

    int j = 0 ;

    int x = 0 ;

    int n = 0 ;

    int str_size = 1 ;

    char string_num[100] ;

    signed char byte = 0 ;

    unsigned char ubyte = 0 ;

    RKString ascii_string = NULL ;

    char* str = RKMem_CArray(1, char) ;

    str[0] = '\0' ;

    string_num[0] = '0' ;

    string_num[1] = '0' ;

    string_num[2] = '0' ;

    string_num[3] = '\0' ;

    while ( i < string->size_in_bytes  ) {

        if ( string->string[i] < 0 ) {

            str = RKMem_Realloc(str, str_size+4, str_size, char, 1) ;

            str_size += 4 ;

            str[str_size-5] = 'u' ;

            str[str_size-4] = 't' ;

            str[str_size-3] = 'f' ;

            str[str_size-2] = '_' ;

            str[str_size-1] = '\0' ;

            byte = string->string[i] ;

            n = 0 ;

            while (byte < 0) {

                byte = byte << 1 ;

                n++ ;
            }

            x = 0 ;

            ubyte = byte = string->string[i] ;

            while ( x < n ) {

                snprintf(string_num, sizeof(string_num), "%u", ubyte) ;

                j = 0 ;

                while ( j < strlen(string_num) ) {

                 str = RKMem_Realloc(str, str_size+1, str_size, char, 1) ;

                 str_size++ ;

                 str[str_size-2] = string_num[j] ;

                 str[str_size-1] = '\0' ;

                 j++ ;

                }

                i++ ;

                if (  i >= string->size_in_bytes ) {

                    break ;
                }

                ubyte = byte = string->string[i] ;

                x++ ;
            }

            i-- ;

        } else {

            str = RKMem_Realloc(str, str_size+1, str_size, char, 1) ;

            str_size++ ;

            str[str_size-2] = string->string[i] ;

            str[str_size-1] = '\0' ;
        }

        i++ ;
    }

    ascii_string = RKString_NewStringFromCString(str) ;

    free(str) ;

    return ascii_string ;
}

static int GetEscapeCharacter( int c ) {

    switch (c) {

        case '\a':

            return 'a' ;

            break;

        case '\b':

            return 'b' ;

            break;

        case '\f':

            return 'f' ;

            break;

        case '\n':

            return 'n' ;

            break;

        case '\r':

            return 'r' ;

            break;

        case '\t':

            return 't' ;

            break;

        case '\v':

            return 'v' ;

            break;

        default:
            break;
    }

    return 'n' ;
}

RKString RKString_SwapEscapeSequencesWithCharacters( RKString string ) {

    int i = 0 ;

    int j = 0 ;

    int x = 0 ;

    int c = 0 ;

    RKULong str_size = 0 ;

    RKULong str2_size = 0 ;

    RKString retstring = NULL ;

    int* str = RKString_GetUTF32String(string, &str_size) ;

    int* str2 = NULL ;

    while ( i < str_size ) {

        c = str[i] ;

        if ( c == '\a' || c == '\b' || c == '\f' || c == '\n'
            || c == '\r' || c == '\t' || c == '\v' ) {

            j = 0 ;

            x = 0 ;

            str2_size = str_size+2 ;

            str2 = RKMem_CArray(str2_size, int) ;

            str2[str2_size-1] = '\0' ;

            while ( j < str2_size ) {

                if ( j != i && j != (i+1) && j != (str2_size-1) ) {

                    str2[j] = str[j+x] ;

                }

                if ( j == i ) {

                    str2[j] = '\\' ;
                }

                if ( j == i+1 ) {

                    str2[j] = GetEscapeCharacter(c) ;

                    x = -1 ;
                }

                if ( j == (str2_size-1) ) {

                    free(str) ;

                    str = str2 ;

                    str_size = str2_size ;

                    i++ ;
                }

                j++ ;
            }
        }

        i++ ;
    }

    RKString_DestroyString(string) ;

    retstring = RKString_NewStringFromUTF32(str, (int)str_size) ;

    free(str) ;

    return retstring ;
}

void* RKAny_NewAny( void* any, RKULong size ) {

    char* ptr = malloc(size) ;

    memcpy(ptr, any, size) ;

    return ptr ;
}

RKStack RKStack_NewStack( void ) {

    RKStack stack = RKMem_NewMemOfType(struct RKStack_s) ;

    stack->list = RKList_NewList() ;

    return stack ;
}

void RKStack_DestroyStack( RKStack stack ) {

    if (stack != NULL) RKList_DeleteList(stack->list) ;

    free(stack) ;
}

void RKStack_Push( RKStack stack, void* data ) {

    RKList_AddToList(stack->list, data) ;
}

void* RKStack_Pop( RKStack stack ) {

    if ( RKStack_IsEmpty(stack) ) return NULL ;

    void* data = RKList_GetData(RKList_GetLastNode(stack->list)) ;

    RKList_DeleteNode(stack->list, RKList_GetLastNode(stack->list)) ;

    return data ;
}

void* RKStack_Peek( RKStack stack ) {

    return RKList_GetData(RKList_GetLastNode(stack->list)) ;
}

int RKStack_IsEmpty( RKStack stack ) {

    return (RKList_GetNumOfNodes(stack->list) == 0) ? 1 : 0 ;
}

RKList RKStack_GetList( RKStack stack ) {

    return stack->list ;
}

RKList_node RKStack_GetListNode( RKStack stack ) {

    return RKList_GetLastNode(stack->list) ;
}
