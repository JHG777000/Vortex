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
#include <string.h>

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

static vortex_int Vortex_IsLittleEndian(void) {
    vortex_ushort n = 0x1;
    return *((vortex_byte*)&n) == 1;
}

static void Vortex_SwapBytes(vortex_byte* ptr, vortex_ulong size) {
    vortex_ulong i = 0;
    while (i < size / 2) {
        vortex_byte t = ptr[i];
        ptr[i] = ptr[size - 1 - i];
        ptr[size - 1 - i] = t;
        i++;
    }
}

static vortex_int Vortex_WriteBytes(VortexFile file, void* data, vortex_ulong size) {
    vortex_byte buffer[16];
    memcpy(buffer, data, size);
    if (Vortex_IsLittleEndian()) {
        Vortex_SwapBytes(buffer, size);
    }
    if (fwrite(buffer, size, 1, file->file) != 1) {
        return -1;
    }
    return 0;
}

static vortex_int Vortex_ReadBytes(VortexFile file, void* data, vortex_ulong size) {
    vortex_byte buffer[16];
    if (fread(buffer, size, 1, file->file) != 1) {
        return -1;
    }
    if (Vortex_IsLittleEndian()) {
        Vortex_SwapBytes(buffer, size);
    }
    memcpy(data, buffer, size);
    return 0;
}

static vortex_byte Vortex_GetBinaryByte(VortexFile file, vortex_int* error) {
    vortex_byte value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_byte)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_sbyte Vortex_GetBinarySByte(VortexFile file, vortex_int* error) {
    vortex_sbyte value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_sbyte)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_short Vortex_GetBinaryShort(VortexFile file, vortex_int* error) {
    vortex_short value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_short)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_ushort Vortex_GetBinaryUShort(VortexFile file, vortex_int* error) {
    vortex_ushort value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_ushort)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_int Vortex_GetBinaryInt(VortexFile file, vortex_int* error) {
    vortex_int value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_int)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_uint Vortex_GetBinaryUInt(VortexFile file, vortex_int* error) {
    vortex_uint value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_uint)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_long Vortex_GetBinaryLong(VortexFile file, vortex_int* error) {
    vortex_long value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_long)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_ulong Vortex_GetBinaryULong(VortexFile file, vortex_int* error) {
    vortex_ulong value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_ulong)) == -1) {
        *error = 1;
        return 0;
    }
    return value;
}

static vortex_float Vortex_GetBinaryFloat(VortexFile file, vortex_int* error) {
    vortex_float value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_float)) == -1) {
        *error = 1;
        return 0.0f;
    }
    return value;
}

static vortex_double Vortex_GetBinaryDouble(VortexFile file, vortex_int* error) {
    vortex_double value;
    if (Vortex_ReadBytes(file, &value, sizeof(vortex_double)) == -1) {
        *error = 1;
        return 0.0;
    }
    return value;
}

static vortex_int Vortex_SetBinaryByte(VortexFile file, vortex_byte value) {
   return Vortex_WriteBytes(file, &value, sizeof(vortex_byte));
}

static vortex_int Vortex_SetBinarySByte(VortexFile file, vortex_sbyte value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_sbyte));
}

static vortex_int Vortex_SetBinaryShort(VortexFile file,  vortex_short value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_short));
}

static vortex_int Vortex_SetBinaryUShort(VortexFile file, vortex_ushort value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_ushort));
}

static vortex_int Vortex_SetBinaryInt(VortexFile file, vortex_int value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_int));
}

static vortex_int Vortex_SetBinaryUInt(VortexFile file, vortex_uint value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_uint));
}

static vortex_int Vortex_SetBinaryLong(VortexFile file, vortex_long value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_long));
}

static vortex_int Vortex_SetBinaryULong(VortexFile file, vortex_ulong value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_ulong));
}

static vortex_int Vortex_SetBinaryFloat(VortexFile file, vortex_float value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_float));
}

static vortex_int Vortex_SetBinaryDouble(VortexFile file, vortex_double value) {
    return Vortex_WriteBytes(file, &value, sizeof(vortex_double));
}

vortex_sbyte VortexFile_Read8BitInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinarySByte(file, error);
}

vortex_int VortexFile_Write8BitInt(VortexFile file, vortex_sbyte value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinarySByte(file, value);
}

vortex_byte VortexFile_Read8BitUInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryByte(file, error);
}

vortex_int VortexFile_Write8BitUInt(VortexFile file, vortex_byte value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryByte(file, value);
}

vortex_short VortexFile_Read16BitInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryShort(file, error);
}

vortex_int VortexFile_Write16BitInt(VortexFile file, vortex_short value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryShort(file, value);
}

vortex_ushort VortexFile_Read16BitUInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryUShort(file, error);
}

vortex_int VortexFile_Write16BitUInt(VortexFile file, vortex_ushort value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryUShort(file, value);
}

vortex_int VortexFile_Read32BitInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryInt(file, error);
}

vortex_int VortexFile_Write32BitInt(VortexFile file, vortex_int value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryInt(file, value);
}

vortex_uint VortexFile_Read32BitUInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryUInt(file, error);
}

vortex_int VortexFile_Write32BitUInt(VortexFile file, vortex_uint value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryUInt(file, value);
}

vortex_long VortexFile_Read64BitInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryLong(file, error);
}

vortex_int VortexFile_Write64BitInt(VortexFile file, vortex_long value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryLong(file, value);
}

vortex_ulong VortexFile_Read64BitUInt(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryULong(file, error);
}

vortex_int VortexFile_Write64BitUInt(VortexFile file, vortex_ulong value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryULong(file, value);
}

vortex_float VortexFile_Read32BitFloat(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryFloat(file, error);
}

vortex_int VortexFile_Write32BitFloat(VortexFile file, vortex_float value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryFloat(file, value);
}

vortex_double VortexFile_Read64BitFloat(VortexFile file, vortex_int* error) {
    if ( file->mode != vortex_read_mode ) {
        *error = -1;
        return 0;
    }
    return Vortex_GetBinaryDouble(file, error);
}

vortex_int VortexFile_Write64BitFloat(VortexFile file, vortex_double value) {
    if ( file->mode != vortex_write_mode ) {
        return -1;
    }
    return Vortex_SetBinaryDouble(file, value);
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
