/*
 Copyright (c) 2017-2025 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexFile.h
//Header file for VortexFile.

#ifndef Vortex_VortexFile_h
#define Vortex_VortexFile_h

typedef struct VortexFile_s* VortexFile ;

typedef enum { 
 vortex_read_mode, 
 vortex_write_mode 
} VortexFileMode;

VortexFile VortexFile_OpenFile(const char* filepath, VortexFileMode mode);
void VortexFile_CloseFile(VortexFile file);
FILE* VortexFile_GetPtrToFile(VortexFile file);
//error is 1 one read/write error, -1 on mode mismatch
vortex_sbyte VortexFile_Read8BitInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write8BitInt(VortexFile file, vortex_sbyte value);
vortex_byte VortexFile_Read8BitUInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write8BitUInt(VortexFile file, vortex_byte value);
vortex_short VortexFile_Read16BitInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write16BitInt(VortexFile file, vortex_short value);
vortex_ushort VortexFile_Read16BitUInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write16BitUInt(VortexFile file, vortex_ushort value);
vortex_int VortexFile_Read32BitInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write32BitInt(VortexFile file, vortex_int num);
vortex_uint VortexFile_Read32BitUInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write32BitUInt(VortexFile file, vortex_uint value);
vortex_long VortexFile_Read64BitInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write64BitInt(VortexFile file, vortex_long value);
vortex_ulong VortexFile_Read64BitUInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write64BitUInt(VortexFile file, vortex_ulong value);
vortex_float VortexFile_Read32BitFloat(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write32BitFloat(VortexFile file, vortex_float num);
vortex_double VortexFile_Read64BitFloat(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write64BitFloat(VortexFile file, vortex_double value);
vortex_int VortexFile_GetUTF32Character(VortexFile file);

#endif /* Vortex_VortexFile_h */
