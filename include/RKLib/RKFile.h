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

//File: RKFile.h

#ifndef RKFile_h
#define RKFile_h

typedef struct RKFile_s* RKFile ;

typedef enum { rk_read_mode, rk_write_mode } RKFileMode ;

void* RKFile_LoadLibraryFromFile( const char* filepath ) ;

int RKFile_FreeLibrary( void* lib ) ;

void* RKFile_GetFunction( void* lib, const char* name ) ;

RKFile RKFile_OpenFile( const char* filepath, RKFileMode mode ) ;

void RKFile_CloseFile( RKFile file ) ;

FILE* RKFile_GetPtrToFile( RKFile file ) ;

RKUInt RKFile_Read32BitInt( RKFile file, RKInt* error ) ;

int RKFile_Write32BitInt( RKFile file, int num ) ;

float RKFile_Read32BitFloat( RKFile file, RKInt* error ) ;

int RKFile_Write32BitFloat( RKFile file, float num ) ;

int RKFile_GetUTF32Character( RKFile file ) ;

#endif /* RKFile_h */
