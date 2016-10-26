/*
 Copyright (c) 2016 Jacob Gordon. All rights reserved.
 
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
#include "RKMem.h"
#include "RKArgs.h"

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
