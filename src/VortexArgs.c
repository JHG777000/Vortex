/*
 Copyright (c) 2016-2024 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexArgs.c
//VortexArgs, allows for dynamically typed variadic functions in C.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexArgs.h>

VortexAny VortexArgs_GetNextArgWithCallback(VortexArgs args,
     const char* typestring, vortex_int is_item,
    VortexAny callback_object, VortexArgsCallback callback) {
   if (!VortexArgs_CanGetNextArgFunc(args,typestring,is_item)) {
       return callback(callback_object);
   }
   return VortexArgs_GetNextArgFunc(args,typestring,is_item);
}

vortex_int VortexArgs_CanGetNextArgFunc(VortexArgs args, const char* typestring, vortex_int is_item) {
   return VortexArgs_CanGetArgWithIndexFunc(args, args->index, typestring, is_item);
}

vortex_int VortexArgs_CanGetArgWithIndexFunc(VortexArgs args, vortex_ulong index, 
      const char* typestring, vortex_int is_item) {
    if (args->verify != 17345432) return 0;
    VortexAny retval = VortexArgs_GetArgWithIndexFunc(args, index, typestring, is_item);
    if ( retval == NULL ) return 0;
    return 1;
}

VortexAny VortexArgs_GetNextArgFunc(VortexArgs args, const char* typestring, vortex_int is_item ) {
    if (args->verify != 17345432) return NULL;
    VortexAny retval = VortexArgs_GetArgWithIndexFunc(args, args->index, typestring, is_item);
    args->index++;
    return retval;
}

VortexAny VortexArgs_GetArgWithIndexFunc(VortexArgs args, vortex_ulong index, 
      const char* typestring, vortex_int is_item) {
    if (args->verify != 17345432) return NULL;
    vortex_int num_of_args = args->array->num_of_bytes / args->array->size_in_bytes;
    vortex_int arg_index = 0;
    while ( arg_index < num_of_args ) {
        VortexArgsBase base = (((VortexArgsBase*)args->array->array)[arg_index]);
        if ( base->verify != 12345423 ) return NULL;
        vortex_int num_of_items = 
          (base->base_type == VortexArgs_Proxy_type) ? base->args->num_of_bytes / base->args->size_in_bytes : 1;
        if ( (index < 0) || (index >= num_of_items) ) {
          if ( index < 0 ) return NULL;
          index -= num_of_items;
          arg_index++;
          continue;
        }
        vortex_ulong i = 0;
        vortex_ulong j = 0;
        VortexAny* type_array = base->type_array;
        vortex_ulong size0 = (vortex_ulong)strlen(typestring);
        vortex_ulong size1 = 0;
        if (type_array == NULL) return NULL;
        while (type_array[i] != NULL) {
            size1 = (vortex_ulong)strlen(type_array[i]);
            if ( size1 == size0 )
                j = !strncmp(type_array[i], typestring, size0);
            if (j) break;
            i++;
        }
        if (!j) return NULL;
        if ( base->base_type == VortexArgs_Dynamic_type ) {
            if (!is_item) return NULL;
            return base;
        }
        if (is_item) return NULL;
        return &((vortex_byte*)base->args->array)[index*base->args->size_in_bytes];
    }
    return NULL;
}

static void* Vortex_CloneArg(VortexArgs args, vortex_ulong* index_ptr, vortex_int* error) {
    vortex_ulong index = *index_ptr;
    vortex_ulong index2 = *index_ptr;
    vortex_ulong num_of_args = args->array->num_of_bytes / args->array->size_in_bytes;
    vortex_ulong arg_index = 0;
    while ( arg_index < num_of_args ) {
        VortexArgsBase base = (((VortexArgsBase*)args->array->array)[arg_index]);
        if ( base->verify != 12345423 ) {
            *error = 1;
            return NULL;
        }
        vortex_ulong num_of_items = 
          (base->base_type == VortexArgs_Proxy_type) ? base->args->num_of_bytes / base->args->size_in_bytes : 1;
        if ( (index < 0) || (index >= num_of_items) ) {
            if ( index < 0 ) {
                *error = 1;
                return NULL;
            }
            index -= num_of_items;
            arg_index++;
            continue;
        }
        vortex_ulong i = 0;
        VortexArgsBase newbase = NULL;
        VortexAny* type_array = base->type_array;
        VortexAny* new_type_array = NULL;
        VortexString string = NULL;
        newbase = base;
        if ( base->base_type == VortexArgs_Dynamic_type ) {
            *index_ptr = (1) + index2;
            return newbase;
        }
        if (type_array == NULL) {
            *error = 1;
            return NULL;
        }
        newbase = vortex_new_mem_of_type(struct VortexArgsBase_s);
        newbase->verify = base->verify;
        newbase->base_type = base->base_type;
        while (type_array[i] != NULL) {
            if ( new_type_array == NULL ) {
                new_type_array = vortex_new_mem_of_type(VortexAny);
            } else {
                new_type_array = vortex_realloc(new_type_array, i+1, i, VortexAny, 1);
            }
            string = VortexString_NewFromCString(type_array[i]);
            new_type_array[i] = VortexString_ConvertToCString(string);
            i++ ;
        }
        new_type_array = vortex_realloc(new_type_array, i+1, i, void*, 1);
        new_type_array[i] = NULL;
        newbase->type_array = new_type_array;
        newbase->args = vortex_new_mem_of_type(struct VortexArgArray_s);
        newbase->args->num_of_bytes = base->args->num_of_bytes;
        newbase->args->size_in_bytes = base->args->size_in_bytes;
        newbase->args->array = vortex_c_array(newbase->args->num_of_bytes, vortex_byte);
        i = 0;
        while (i < newbase->args->num_of_bytes) {
            ((vortex_byte*)newbase->args->array)[i] = ((vortex_byte*)base->args->array)[i];
            i++;
        }
        *index_ptr = (base->args->num_of_bytes / base->args->size_in_bytes) + index2;
        return newbase;
    }
    return NULL;
}

VortexArgs VortexArgs_CloneArgs(VortexArgs args) {
    vortex_int i = 0;
    vortex_ulong index = 0;
    vortex_int error = 0;
    VortexAny retptr = &index;
    if (args->verify != 17345432) return NULL;
    VortexArgs newargs = vortex_new_mem_of_type(struct VortexArgs_s);
    newargs->index = 0;
    newargs->verify = args->verify;
    newargs->array = vortex_new_mem_of_type(struct VortexArgArray_s);
    newargs->array->num_of_bytes = args->array->num_of_bytes;
    newargs->array->size_in_bytes = args->array->size_in_bytes;
    newargs->array->array = vortex_c_array(args->array->num_of_bytes, vortex_byte);
    while ( retptr != NULL ) {
        retptr = Vortex_CloneArg(args, &index, &error);
        if ( retptr != NULL ) ((VortexArgsBase*)newargs->array->array)[i] = ((VortexArgsBase)retptr);
        i++;
    }
    if ( (retptr == NULL) && (error) ) {
        free(newargs->array->array);
        free(newargs->array);
        free(newargs);
        return NULL;
    }
    newargs->cloned = 1;
    return newargs;
}

static vortex_int Vortex_DestroyArg(VortexArgs args, vortex_ulong index) {
    if (args->verify != 17345432) return 1;
    vortex_ulong num_of_args = args->array->num_of_bytes / args->array->size_in_bytes;
    if ( num_of_args > 0 ) {
        VortexArgsBase base = (((VortexArgsBase*)args->array->array)[0]);
        if ( base->verify != 12345423 ) return 1;
        vortex_ulong i = 1;
        VortexAny* new_arg_base_array = NULL;
        new_arg_base_array = vortex_c_array(num_of_args-1, VortexAny);
        while (i <= (num_of_args-1)) {
            new_arg_base_array[i-1] = ((VortexAny*)args->array->array)[i];
            i++ ;
        }
        free(args->array->array);
        args->array->array = new_arg_base_array;
        args->array->num_of_bytes -= args->array->size_in_bytes;
        if ( base->base_type == VortexArgs_Dynamic_type ) return 0;
        i = 0;
        VortexAny* type_array = base->type_array;
        while (type_array[i] != NULL) {
            free(type_array[i]);
            i++;
        }
        free(type_array);
        free(base->args->array);
        free(base->args);
        free(base);
        return 0;
    }
    return -1;
}

vortex_int VortexArgs_DestroyClonedArgs(VortexArgs args) {
    if ( !args->cloned ) return 0;
    vortex_int retval = 0;
    while ((!retval) && (retval >= 0)) {
        retval = Vortex_DestroyArg(args, 0);
    }
    if ( retval == 1 ) return 0;
    free(args->array->array);
    free(args->array);
    free(args);
    return 1;
}
