/*
 Copyright (c) 2025 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexModule.c

#include <stdio.h>
#include <stdlib.h>

#if  !_WIN32
#include <dlfcn.h>
#endif

#include <Vortex/VortexMem.h>
#include <Vortex/VortexFile.h>

 #ifdef _WIN32
  #include <windows.h>
  #define vortex_dlopen(file) LoadLibrary(file)
  #define vortex_dlclose(library) FreeLibrary((HMODULE)library)
  #define vortex_dlsym(library, name) GetProcAddress((HMODULE)library,name)
 #else
  #define vortex_dlopen(file) dlopen(file, RTLD_LAZY | RTLD_LOCAL)
  #define vortex_dlclose(library) dlclose(library)
  #define vortex_dlsym(library, name) dlsym(library, name)
 #endif

VortexAny VortexModule_LoadLibrary(const char* filepath) {
    return vortex_dlopen(filepath);
}

vortex_int VortexModule_FreeLibrary(VortexAny library) {
    return vortex_dlclose(library) ;
}

VortexAny VortexModule_GetFunction(VortexAny library, const char* name) {
     return vortex_dlsym(library, name);
}