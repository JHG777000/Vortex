/*
 Copyright (c) 2017-2019 Jacob Gordon. All rights reserved.

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

//File: RKFile.c

#include <stdio.h>
#include <stdlib.h>

#if  !_WIN32
#include <dlfcn.h>
#endif

#include <RKLib/RKMem.h>
#include <RKLib/RKFile.h>

struct RKFile_s { FILE* file ; RKFileMode mode ; } ;

 #ifdef _WIN32

  #include <windows.h>

  #define rkdlopen(file) LoadLibrary(file)

  #define rkdlclose(lib) FreeLibrary((HMODULE)lib)

  #define rkdlsym(lib, name) GetProcAddress((HMODULE)lib,name)

 #else

  #define rkdlopen(file) dlopen(file, RTLD_LAZY | RTLD_LOCAL)

  #define rkdlclose(lib) dlclose(lib)

  #define rkdlsym(lib, name) dlsym(lib, name)

 #endif

void* RKFile_LoadLibraryFromFile( const char* filepath ) {

    return rkdlopen(filepath) ;
}

int RKFile_FreeLibrary( void* lib ) {

    return rkdlclose(lib) ;
}

void* RKFile_GetFunction( void* lib, const char* name ) {

     return rkdlsym(lib, name) ;
}

RKFile RKFile_OpenFile( const char* filepath, RKFileMode mode ) {

    RKFile file = RKMem_NewMemOfType(struct RKFile_s) ;

    if ( mode == rk_read_mode ) file->file = fopen(filepath, "r") ;

    if ( mode == rk_write_mode ) file->file = fopen(filepath, "w") ;

    file->mode = mode ;

    return file ;
}

void RKFile_CloseFile( RKFile file ) {

    fclose(file->file) ;

    free(file) ;
}

FILE* RKFile_GetPtrToFile( RKFile file ) {

    return file->file ;
}

static RKUInt Get32BitInt( FILE* file, RKInt* error ) {

    static RKUInt table[] = {1,256,65536,16777216} ;

    RKInt i = 0 ;

    RKInt input = 0 ;

    RKInt errorval = 0 ;

    RKUInt numsum = 0 ;

    while ( i < 4 ) {

        input = getc(file) ;

        if (input == EOF){

            errorval++ ;

            break ;
        }

        numsum += (input * table[i]) ;

        i++ ;
    }

    *error = errorval ;

    return numsum ;

}

static RKInt Set32BitInt( FILE* file, RKUInt num ) {

    static RKUInt table[] = {1,256,65536,16777216} ;

    char array[4] ;

    RKInt i = 0 ;

    RKInt j = 3 ;

    RKInt error = 0 ;

    RKUInt output = 0 ;

    while ( j >= 0 ) {

        if ( num < table[j] ) {

         array[j] = 0 ;

        } else {

         output = num / table[j] ;

         array[j] = output ;

         num -= (output * table[j]) ;

        }

        j-- ;
    }

    while ( i < 4 ) {

        error = putc(array[i],file) ;

        if (error == EOF) break ;

        i++ ;
    }

    return (error < 0) ? 1 : 0 ;

}

static float Get32BitFloat( FILE* file, RKInt* error ) {

    RKUInt i = Get32BitInt(file, error) ;

    return *((float*)&i) ;

}

static RKInt Set32BitFloat( FILE* file, float num ) {

    RKUInt i = *((RKUInt*)&num) ;

    return Set32BitInt(file, i) ;
}


RKUInt RKFile_Read32BitInt( RKFile file, RKInt* error ) {

    if ( file->mode != rk_read_mode ) {

        *error = -1 ;

        return 0 ;
    }

    return Get32BitInt(file->file, error) ;
}

int RKFile_Write32BitInt( RKFile file, int num ) {

    if ( file->mode != rk_write_mode ) {

        return -1 ;
    }

   return Set32BitInt(file->file, num) ;
}

float RKFile_Read32BitFloat( RKFile file, RKInt* error ) {

    if ( file->mode != rk_read_mode ) {

        *error = -1 ;

        return 0 ;
    }

    return Get32BitFloat(file->file, error) ;
}

int RKFile_Write32BitFloat( RKFile file, float num ) {

    if ( file->mode != rk_write_mode ) {

        return -1 ;
    }

    return Set32BitFloat(file->file, num) ;
}

int RKFile_GetUTF32Character( RKFile file ) {

    if ( file->mode != rk_read_mode ) {

        return -1 ;
    }

    int n = 0 ;

    signed char byte = 0 ;

    unsigned int byte0 = 0 ;

    unsigned int byte1 = 0 ;

    unsigned int byte2 = 0 ;

    unsigned int byte3 = 0 ;

    unsigned int value = 0 ;

    signed char basebyte = 0 ;

loop:

    byte = fgetc(file->file) ;

    if ( byte == EOF ) return byte ;

    basebyte = byte ;

    if ( byte > 0 ) {

        n = 1 ;

    } else {

        n = 0 ;

        while (byte < 0) {

            byte = byte << 1 ;

            n++ ;
        }

        if ( n == 1 ) {

            goto loop ;
        }

    }

    if ( n == 1 ) {

        byte0 = basebyte ;

        value = byte0 ;
    }

    if ( n == 2 ) {

        byte0 = basebyte ;

        byte0 = byte0 & 0x3F ;

        byte0 = byte0 << 6 ;

        byte1 = fgetc(file->file) ;

        byte1 = byte1 & 0x7F ;

        value = byte0 | byte1 ;
    }

    if ( n == 3 ) {

        byte0 = basebyte ;

        byte0 = byte0 & 0x1F ;

        byte0 = byte0 << 12 ;

        byte1 = fgetc(file->file) ;

        byte1 = byte1 & 0x7F ;

        byte1 = byte1 << 6 ;

        byte2 = fgetc(file->file) ;

        byte2 = byte2 & 0x7F ;

        value = byte0 | byte1 | byte2 ;

    }

    if ( n == 4 ) {

        byte0 = basebyte ;

        byte0 = byte0 & 0xF ;

        byte0 = byte0 << 18 ;

        byte1 = fgetc(file->file) ;

        byte1 = byte1 & 0x7F ;

        byte1 = byte1 << 12 ;

        byte2 = fgetc(file->file) ;

        byte2 = byte2 & 0x7F ;

        byte2 = byte2 << 6 ;

        byte3 = fgetc(file->file) ;

        byte3 = byte3 & 0x7F ;

        value = byte0 | byte1 | byte2 | byte3 ;

    }

    return value ;

}
