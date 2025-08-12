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
vortex_uint VortexFile_Read32BitInt(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write32BitInt(VortexFile file, vortex_int num);
vortex_float VortexFile_Read32BitFloat(VortexFile file, vortex_int* error);
vortex_int VortexFile_Write32BitFloat(VortexFile file, vortex_float num);
vortex_int VortexFile_GetUTF32Character(VortexFile file);

#endif /* Vortex_VortexFile_h */
