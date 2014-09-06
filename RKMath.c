/*
 Copyright (c) 2014 Jacob Gordon. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: RKMath.c
//RKMath, a n-dimensional vector math library designed to take advantage of autovectorization.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "RKMath.h"

 void RKMath_Add(float outvec[], const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        outvec[i] = vec_a[i] + vec_b[i] ;
        
        i++ ;
    }
}

 void RKMath_Sub(float outvec[], const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        outvec[i] = vec_a[i] - vec_b[i] ;
        
        i++ ;
    }
}

 void RKMath_Mul(float outvec[], const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        outvec[i] = vec_a[i] * vec_b[i] ;
        
        i++ ;
    }
}

 void RKMath_Div(float outvec[], const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        outvec[i] = vec_a[i] / vec_b[i] ;
        
        i++ ;
    }
}

void RKMath_Equal(float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        vec_a[i] = vec_b[i] ;
        
        i++ ;
    }
}

void RKMath_Set_Vec_Equal_To_A_Const(float vec[], const float const__, const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        vec[i] = const__ ;
        
        i++ ;
    }
}

int RKMath_IsEqual(const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        if ( vec_a[i] != vec_b[i] ) return 0 ;
        
        i++ ;
    }
    
    return 1 ;
}

 void RKMath_Neg(float vec[], const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        vec[i] = -vec[i] ;
        
        i++ ;
    }
}

 float RKMath_Abs(const float vec[], const int size) {
    
    int i = 0 ;
    
    float retval = 0 ;
    
    while ( i < size ) {
        
        retval = powf(vec[i],2) + retval ;
        
        i++ ;
    }
    
    return sqrtf(retval) ;
}

 float RKMath_Dot(const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    float retval = 0 ;
    
    while ( i < size ) {
        
       retval = ( vec_a[i] * vec_b[i] ) + retval ;
        
        i++ ;
    }
    
    return retval ;
}

 void RKMath_Cross(float outvec[], const float vec_a[], const float vec_b[] ) {
    
    RKMath_Vectorthis(outvec, vec_a[1] * vec_b[2] - vec_a[2] * vec_b[1], vec_a[2] * vec_b[0] - vec_a[0] * vec_b[2], vec_a[0] * vec_b[1] - vec_a[1] * vec_b[0]) ;
}

void RKMath_Norm(float outvec[], float vec[], const int size) {
    
    RKMath_NewVector(One_Div_By_ABS_Vec, size) ;
    
    const float abs = RKMath_Abs(vec, size) ;
    
    if ( abs != 0 ) {
        
        RKMath_Set_Vec_Equal_To_A_Const(One_Div_By_ABS_Vec, 1.0f / abs, size) ;
        
        RKMath_Mul(outvec, vec, One_Div_By_ABS_Vec, size) ;
        
    } else {
        
        RKMath_Equal(outvec, vec, size) ;
    }
}

 float RKMath_Distance(const float vec_a[], const float vec_b[], const int size) {
    
    int i = 0 ;
    
    float retval = 0 ;
    
    while ( i < size ) {
        
        retval = powf((vec_b[i] - vec_a[i]),2) + retval ;
        
        i++ ;
    }
    
    return sqrtf(retval) ;
}

 void RKMath_Clamp(float vec[], const float min, const float max, const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        vec[i] = ( vec[i] > max ) ? max : vec[i] ;
        
        vec[i] = ( vec[i] < min ) ? min : vec[i] ;
        
        i++ ;
    }
    
}

