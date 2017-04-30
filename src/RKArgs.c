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

//File: RKArgs.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <RKLib/RKMem.h>
#include <RKLib/RKArgs.h>

void* RKArgs_GetNextArgFunc( RKArgs args, const char* typestring, int is_item ) {
    
    if (args->verify != 17345432) return NULL ;
    
    void* retval = RKArgs_GetArgWithIndexFunc(args, args->index, typestring, is_item) ;
    
    args->index++ ;
    
    return retval ;
}

void* RKArgs_GetArgWithIndexFunc( RKArgs args, int index, const char* typestring, int is_item ) {
    
    if (args->verify != 17345432) return NULL ;
    
    int num_of_args = args->array->num_of_bytes / args->array->size_in_bytes ;
    
    int arg_index = 0 ;
    
    while ( arg_index < num_of_args ) {
        
        RKArgsBase base = (((RKArgsBase*)args->array->array)[arg_index]) ;
        
        if ( base->verify != 12345423 ) return NULL ;
        
        int num_of_items = (base->base_type == RKArgs_Proxy_type) ? base->args->num_of_bytes / base->args->size_in_bytes : 1 ;
        
        if ( (index < 0) || (index >= num_of_items) ) {
           
          if ( index < 0 ) return NULL ;
            
          index -= num_of_items ;
                
          arg_index++ ;
                
          continue ;
            
        }
        
        int i = 0 ;
        
        int j = 0 ;
        
        void** type_array = base->type_array ;
        
        int size0 = (int)strlen(typestring) ;
        
        int size1 = 0 ;
        
        if (type_array == NULL) return NULL ;
        
        while (type_array[i] != NULL) {
            
            size1 = (int)strlen(type_array[i]) ;
            
            if ( size1 == size0 )
                j = !strncmp(type_array[i], typestring, size0) ;
            
            if (j) break ;
            
            i++ ;
        }
        
        if (!j) return NULL ;
        
        if ( base->base_type == RKArgs_Dynamic_type ) {
            
            if (!is_item) return NULL ;
            
            return base ;
        }
        
        if (is_item) return NULL ;
        
        return &((RKByte*)base->args->array)[index*base->args->size_in_bytes] ;
    }
    
    return NULL ;
}

static void* CloneArg( RKArgs args, int* index_ptr, int* error ) {
    
    int index = *index_ptr ;
    
    int index2 = *index_ptr ;
    
    int num_of_args = args->array->num_of_bytes / args->array->size_in_bytes ;
    
    int arg_index = 0 ;
    
    while ( arg_index < num_of_args ) {
        
        RKArgsBase base = (((RKArgsBase*)args->array->array)[arg_index]) ;
        
        if ( base->verify != 12345423 ) {
            
            *error = 1 ;
            
            return NULL ;
        }
        
        int num_of_items = (base->base_type == RKArgs_Proxy_type) ? base->args->num_of_bytes / base->args->size_in_bytes : 1 ;
        
        if ( (index < 0) || (index >= num_of_items) ) {
            
            if ( index < 0 ) {
                
                *error = 1 ;
                
                return NULL ;
            }
            
            index -= num_of_items ;
            
            arg_index++ ;
            
            continue ;
        }
        
        int i = 0 ;
        
        RKArgsBase newbase = NULL ;
        
        void** type_array = base->type_array ;
        
        void** new_type_array = NULL ;
        
        RKString string = NULL ;
        
        newbase = base ;
        
        if ( base->base_type == RKArgs_Dynamic_type ) {
            
            *index_ptr = (1) + index2 ;
            
            return newbase ;
        }
        
        if (type_array == NULL) {
            
            *error = 1 ;
            
            return NULL ;
        }
        
        newbase = RKMem_NewMemOfType(struct RKArgsBase_s) ;
        
        newbase->verify = base->verify ;
        
        newbase->base_type = base->base_type ;
        
        while (type_array[i] != NULL) {
            
            if ( new_type_array == NULL ) {
                
                new_type_array = RKMem_NewMemOfType(void*) ;
                
            } else {
                
                new_type_array = RKMem_Realloc(new_type_array, i+1, i, void*, 1) ;
            }
            
            string = RKString_NewString(type_array[i]) ;
            
            new_type_array[i] = RKString_ConvertToCString(string) ;
            
            i++ ;
        }
        
        new_type_array = RKMem_Realloc(new_type_array, i+1, i, void*, 1) ;
        
        new_type_array[i] = NULL ;
        
        newbase->type_array = new_type_array ;
        
        newbase->args = RKMem_NewMemOfType(struct RKArgArray_s) ;
        
        newbase->args->num_of_bytes = base->args->num_of_bytes ;
        
        newbase->args->size_in_bytes = base->args->size_in_bytes ;
        
        newbase->args->array = RKMem_CArray(newbase->args->num_of_bytes, RKByte) ;
        
        i = 0 ;
        
        while (i < newbase->args->num_of_bytes) {
            
            ((RKByte*)newbase->args->array)[i] = ((RKByte*)base->args->array)[i] ;
            
            i++ ;
        }
        
        *index_ptr = (base->args->num_of_bytes / base->args->size_in_bytes) + index2 ;
        
        return newbase ;
    }
    
    return NULL ;
}

RKArgs RKArgs_CloneArgs( RKArgs args ) {
   
    int i = 0 ;
    
    int index = 0 ;
    
    int error = 0 ;
    
    void* retptr = &index ;
    
    if (args->verify != 17345432) return NULL ;
    
    RKArgs newargs = RKMem_NewMemOfType(struct RKArgs_s) ;
    
    newargs->index = 0 ;
    
    newargs->verify = args->verify ;
    
    newargs->array = RKMem_NewMemOfType(struct RKArgArray_s) ;
    
    newargs->array->num_of_bytes = args->array->num_of_bytes ;
    
    newargs->array->size_in_bytes = args->array->size_in_bytes ;
    
    newargs->array->array = RKMem_CArray(args->array->num_of_bytes, RKByte) ;
    
    while ( retptr != NULL ) {
        
        retptr = CloneArg(args, &index, &error) ;
        
        if ( retptr != NULL ) ((RKArgsBase*)newargs->array->array)[i] = ((RKArgsBase)retptr) ;
          
        i++ ;
    }
    
    if ( (retptr == NULL) && (error) ) {
        
        free(newargs->array->array) ;
        
        free(newargs->array) ;
        
        free(newargs) ;
        
        return NULL ;
    }
    
    newargs->cloned = 1 ;
    
    return newargs ;
}

static int DestroyArg( RKArgs args, int index ) {
    
    if (args->verify != 17345432) return 1 ;
    
    int num_of_args = args->array->num_of_bytes / args->array->size_in_bytes ;
    
    if ( num_of_args > 0 ) {
        
        RKArgsBase base = (((RKArgsBase*)args->array->array)[0]) ;
        
        if ( base->verify != 12345423 ) return 1 ;
        
        int i = 1 ;
        
        void** new_arg_base_array = NULL ;
        
        new_arg_base_array = RKMem_CArray(num_of_args-1, void*) ;
        
        while (i <= (num_of_args-1)) {
            
            new_arg_base_array[i-1] = ((void**)args->array->array)[i] ;
            
            i++ ;
        }
        
        free(args->array->array) ;
        
        args->array->array = new_arg_base_array ;
        
        args->array->num_of_bytes -= args->array->size_in_bytes ;
        
        if ( base->base_type == RKArgs_Dynamic_type ) return 0 ;
        
        i = 0 ;
        
        void** type_array = base->type_array ;
        
        while (type_array[i] != NULL) {
            
            free(type_array[i]) ;
            
            i++ ;
        }
        
        free(type_array) ;
        
        free(base->args->array) ;
        
        free(base->args) ;
        
        free(base) ;
        
        return 0 ;
    }

    return -1 ;
}

int RKArgs_DestroyClonedArgs( RKArgs args ) {
    
    if ( !args->cloned ) return 0 ;
    
    int retval = 0 ;
    
    while ((!retval) && (retval >= 0)) {
        
        retval = DestroyArg(args, 0) ;
    }
    
    if ( retval == 1 ) return 0 ;
    
    free(args->array->array) ;
    
    free(args->array) ;
    
    free(args) ;
    
    return 1 ;
}
