/*
 Copyright (c) 2017-2025 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexFile.c

#include <stdio.h>
#include <stdlib.h>

#include <Vortex/VortexMem.h>
#include <Vortex/VortexFile.h>

struct VortexFile_s { 
 FILE* file;
 VortexFileMode mode; 
};

VortexFile VortexFile_OpenFile(const char* filepath, VortexFileMode mode) {
    VortexFile file = vortex_new_mem_of_type(struct VortexFile_s);
    if ( mode == vortex_read_mode ) file->file = fopen(filepath, "r");
    if ( mode == vortex_write_mode ) file->file = fopen(filepath, "w");
    file->mode = mode;
    return file;
}

void VortexFile_CloseFile(VortexFile file) {
    fclose(file->file);
    free(file);
}

FILE* VortexFile_GetPtrToFile(VortexFile file) {
    return file->file;
}

static vortex_uint Get32BitInt(FILE* file, vortex_int* error) {
    static vortex_uint table[] = {1,256,65536,16777216};
    vortex_int i = 0;
    vortex_int input = 0;
    vortex_int errorval = 0;
    vortex_uint numsum = 0;
    while (i < 4) {
        input = getc(file);
        if (input == EOF){
            errorval++;
            break;
        }
        numsum += (input * table[i]);
        i++;
    }
    *error = errorval;
    return numsum;

}

static vortex_int Set32BitInt(FILE* file, vortex_uint num) {
    static vortex_uint table[] = {1,256,65536,16777216};
    vortex_byte array[4];
    vortex_int i = 0;
    vortex_int j = 3;
    vortex_int error = 0;
    vortex_uint output = 0;
    while (j >= 0) {
        if ( num < table[j] ) {
         array[j] = 0;
        } else {
         output = num / table[j];
         array[j] = output;
         num -= (output * table[j]);
        }
        j-- ;
    }
    while ( i < 4 ) {
        error = putc(array[i],file);
        if (error == EOF) break;
        i++ ;
    }
    return (error < 0) ? 1 : 0;
}

static float Get32BitFloat( FILE* file, vortex_int* error) {
    vortex_uint i = Get32BitInt(file, error);
    return *((float*)&i);
}

static vortex_int Set32BitFloat(FILE* file, vortex_float num) {
    vortex_uint i = *((vortex_uint*)&num);
    return Set32BitInt(file, i);
}


vortex_uint VortexFile_Read32BitInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Get32BitInt(file->file, error);
}

vortex_int VortexFile_Write32BitInt(VortexFile file, vortex_int num) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
   return Set32BitInt(file->file, num);
}

vortex_float VortexFile_Read32BitFloat(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Get32BitFloat(file->file, error);
}

vortex_int VortexFileFile_Write32BitFloat(VortexFile file, vortex_float num) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Set32BitFloat(file->file, num) ;
}

vortex_int VortexFile_GetUTF32Character(VortexFile file) {
    if ( file->mode != vortex_read_mode ) {
        return -1;
    }
    vortex_int n = 0;
    vortex_sbyte byte = 0;
    vortex_uint byte0 = 0;
    vortex_uint byte1 = 0;
    vortex_uint byte2 = 0;
    vortex_uint byte3 = 0;
    vortex_uint value = 0;
    vortex_sbyte basebyte = 0;
loop:
    byte = fgetc(file->file);
    if ( byte == EOF ) return byte;
    basebyte = byte;
    if ( byte > 0 ) {
        n = 1;
    } else {
        n = 0;
        while (byte < 0) {
            byte = byte << 1;
            n++;
        }
        if ( n == 1 ) {
            goto loop;
        }
    }
    if ( n == 1 ) {
        byte0 = basebyte;
        value = byte0;
    }
    if ( n == 2 ) {
        byte0 = basebyte;
        byte0 = byte0 & 0x3F;
        byte0 = byte0 << 6;
        byte1 = fgetc(file->file);
        byte1 = byte1 & 0x7F;
        value = byte0 | byte1;
    }
    if ( n == 3 ) {
        byte0 = basebyte;
        byte0 = byte0 & 0x1F;
        byte0 = byte0 << 12;
        byte1 = fgetc(file->file);
        byte1 = byte1 & 0x7F;
        byte1 = byte1 << 6;
        byte2 = fgetc(file->file);
        byte2 = byte2 & 0x7F;
        value = byte0 | byte1 | byte2;
    }
    if ( n == 4 ) {
        byte0 = basebyte;
        byte0 = byte0 & 0xF;
        byte0 = byte0 << 18;
        byte1 = fgetc(file->file);
        byte1 = byte1 & 0x7F;
        byte1 = byte1 << 12;
        byte2 = fgetc(file->file);
        byte2 = byte2 & 0x7F;
        byte2 = byte2 << 6;
        byte3 = fgetc(file->file);
        byte3 = byte3 & 0x7F;
        value = byte0 | byte1 | byte2 | byte3;
    }
    return value;
}
