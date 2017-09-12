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

//File: RKMem.c
//RKMem, provides some nice macros and a funtion to make dealing with memory a little easier in C.
//As well as a double linked-list implementation, RKList.
//And a dictionary type, RKStore.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <RKLib/RKMem.h>

 struct RKStack_s { RKList list ; } ;

 struct RKString_s { RKULong size ; char* string ; } ;

 struct RKIndex_s { RKStore store ; int max_num_of_items ; int num_of_items ; } ;

 typedef struct RKStore_letter_s { RKList_node node ; struct RKStore_letter_s* next_alphabet ; } RKStore_letter ;

 struct RKStore_s { RKStore_letter* dictionary ; RKList items ; } ;

 struct RKList_node_s { struct RKList_node_s* before ; struct RKList_node_s* after ; void* data ; } ;

 struct RKList_s { int num_of_nodes ; RKList_node first ; RKList_node last ; } ;

 void* RKMem_Realloc_Func(void* data, size_t newsize, size_t oldsize, int NULLonError0No1Yes) {
    
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

RKList RKList_NewList( void ) {
    
    RKList newlist = RKMem_NewMemOfType(struct RKList_s) ;
    
    newlist->num_of_nodes = 0 ;
    
    newlist->first = NULL ;
    
    newlist->last = NULL ;
    
    return newlist ;

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
    
    list->num_of_nodes++ ;
    
    return list->last ;
}

RKList_node RKList_AddNodeToList( RKList_node node, RKList list ) {
    
    return RKList_AddToList(list, node->data) ;
}

void RKList_NodeSwap( RKList list, RKList_node node_a, RKList_node node_b ) {
    
    RKList_node Before = node_a->before ;
    
    RKList_node After = node_a->after ;
    
    node_a->before = node_b->before ;
    
    node_a->after = node_b->after ;
    
    if ( node_a->before != NULL ) node_a->before->after = node_a ;
    
    if ( node_a->after != NULL ) node_a->after->before = node_a ;
    
    if ( node_a->before == NULL ) list->first = node_a ;
    
    if ( node_a->after == NULL ) list->last = node_a ;
    
    node_b->before = Before ;
    
    node_b->after = After ;
    
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
    
    node_a->before = node_b->before ;
    
    node_a->after = node_b ;
    
    node_b->before->after = node_a ;
    
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

RKList_node RKList_GetNextNode(RKList_node node) {
    
    if ( node == NULL ) return NULL ;
    
    return node->after ;
}

RKList_node RKList_GetPreviousNode(RKList_node node) {
    
    if ( node == NULL ) return NULL ;
    
    return node->before ;
}

RKList_node RKList_GetFirstNode(RKList list) {
    
    return list->first ;
}

RKList_node RKList_GetLastNode(RKList list) {
    
    return list->last ;
}

int RKList_GetNumOfNodes(RKList list) {
    
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

RKList_node RKList_GetNextNodeAfterN( RKList_node node, int n ) {
    
    int i = 0 ;
    
    while ( i < n ) {
        
        if ( node == NULL ) return NULL ;
        
        node = node->after ;
        
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

static RKList_node RKS_GetSetNode( RKStore store, const char* label, RKList_node node, int flag ) {
    
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
                        
                        current_alphabet[value].node = NULL ;
                        
                        return retnode ;
                    }
                    
                } else {
                    
                    current_alphabet[value].node = node ;
                    
                    return current_alphabet[value].node ;
                    
                }
                
            } else {
                
                if ( current_alphabet[value].next_alphabet == NULL ) current_alphabet[value].next_alphabet = RKS_NewBinaryNode() ;
                
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
    
    RKList_node node = RKS_GetSetNode(store, label, NULL, 0) ;
    
    if ( node == NULL ) return 0 ;
    
    RKList_SetData(node, item) ;
    
    return 1 ;
}

int RKStore_AddItem( RKStore store, void* item, const char* label ) {
    
    if ( !(RKStore_ItemExists(store, label)) ) {
    
        RKList_node node = RKS_AddItem(store, item) ;
    
        node = RKS_GetSetNode(store, label, node, 0) ;
        
        if ( node == NULL ) return 0 ;
        
    } else {
        
        return RKS_StoreItem(store, item, label) ;
    }
    
    return 0 ;
}

int RKStore_RemoveItem( RKStore store, const char* label ) {
    
    RKList_node node = RKS_GetSetNode(store, label, NULL, 1) ;
    
    if ( node == NULL ) return 0 ;
    
    RKList_DeleteNode(store->items, node) ;
    
    return 1 ;
}

void* RKStore_GetItem( RKStore store, const char* label ) {
    
    RKList_node node = RKS_GetSetNode(store, label, NULL, 0) ;
    
    if ( node == NULL ) return NULL ;
    
    return RKList_GetData(node) ;
}

int RKStore_ItemExists( RKStore Store, const char* label ) {
    
    RKList_node node = RKS_GetSetNode(Store, label, NULL, 0) ;
    
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
    
    return store->items ;
}

void RKStore_IterateStoreWith( RKMemIteratorFuncType iterator, RKStore store ) {
    
    if (store->items != NULL) RKList_IterateListWith(iterator, store->items) ;
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
    
    int index = rkindex->num_of_items + 1 ;
    
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

RKString RKString_NewStringFromBuffer( const char* text, size_t size_in_bytes ) {
    
    RKString string = RKMem_NewMemOfType(struct RKString_s) ;
    
    string->size = size_in_bytes ;
    
    string->string = RKMem_CArray(string->size, char) ;
    
    string->string[string->size-1] = '\0' ;
    
    int i = 0 ;
    
    while ( i < string->size-1 ) {
        
        string->string[i] = text[i] ;
        
        i++ ;
    }
    
    string->size -= 1 ; //not counting '\0'
    
    return string ;
}

RKString RKString_NewStringFromCString( const char* text ) {
    
    RKString string = RKMem_NewMemOfType(struct RKString_s) ;
    
    string->size = strlen(text) ;
    
    string->string = RKMem_CArray(string->size+1, char) ;
    
    strcpy(string->string, text) ;
    
    return string ;
}

RKString RKString_NewStringFromTwoStrings( RKString a, RKString b ) {
    
    RKString string = RKMem_NewMemOfType(struct RKString_s) ;
    
    string->size = a->size + b->size ;
    
    string->string = RKMem_CArray(string->size+1, char) ;
    
    int i = 0 ;
    
    int j = 0 ;
    
    while ( j < a->size ) {
        
        string->string[i] = a->string[j] ;
        
        i++ ;
        
        j++ ;
    }

    j = 0 ;
    
    while ( j < b->size ) {
        
        string->string[i] = b->string[j] ;
        
        i++ ;
        
        j++ ;
    }
    
    string->string[i] = '\0' ;
    
    return string ;
}

void RKString_DestroyString( RKString string ) {
    
    if ( string == NULL ) return ;
    
    free(string->string) ;
    
    free(string) ;
}

RKULong RKString_GetSize( RKString string ) {
    
    return string->size ;
}

char* RKString_GetString( RKString string ) {
    
    if ( string == NULL ) return NULL ;
    
    return string->string ;
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

char* RKString_ConvertToCString( RKString string ) {
    
    char* str = string->string ;
    
    free(string) ;
    
    return str ;
}

void RKString_PrintString( RKString string ) {
    
    printf("%s", string->string) ;
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