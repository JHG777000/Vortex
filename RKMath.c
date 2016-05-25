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

//File: RKMath.c
//RKMath, a n-dimensional vector math library designed to take advantage of autovectorization.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "RKMem.h"
#include "RKMath.h"

 void RKMath_SeedRandomState( RKMath_RandState* randstate, int seed ) {
    
     randstate->init = 1 ;
     
     randstate->state_a = (unsigned)time(NULL) + seed ;
 }

 int RKMath_ARandomNumber( RKMath_RandState* randstate, int randmin, int randmax ) {
    
    int randval = 0 ;
    
    if ( randmax == -1 ) randstate->init = 0 ;
    
    randmax++ ;
    
    if ( randmax <= 0 ) randmax = 1 ;
    
    if (!randstate->init) {
        
        randstate->state_a = (unsigned)time(NULL) ;
        
        randstate->init = 1 ;
    }
    
    randstate->state_a ^= (randstate->state_a << 21) ;
    
    randstate->state_a ^= (randstate->state_a >> 35) ;
    
    randstate->state_a ^= (randstate->state_a << 4) ;
    
    randval = (int) randstate->state_a % randmax ;
    
    randval = (randval < 0) ? -randval : randval ;
    
    randval = (randval < randmin) ? RKMath_ARandomNumber(randstate, randmin, randmax) : randval ;
    
    return  randval ;
    
}

//Is it more random?

int RKMath_AMoreRandomNumber( RKMath_RandState* randstate, int randmin, int randmax ) {
    
    int randval = 0 ;
    
    if ( randmax <= 0 ) randmax = 1 ;
    
    if (!randstate->init_) {
        
        randstate->init_ = 1 ;
        
        randstate->update = 10 ;
        
        randstate->seconds = (unsigned)time(NULL) ;
        
        randstate->gate = RKMath_ARandomNumber(randstate, 0, 1) ;
        
        randstate->state_b = (unsigned)time(NULL) + RKMath_ARandomNumber(randstate, 0, 743276439) ;
    }
    
    if ( ((unsigned)time(NULL) - randstate->seconds) >= randstate->update ) {
        
        if (randstate->gate) {
            
         randstate->seconds = (unsigned)time(NULL) ;
        
         randstate->gate = RKMath_ARandomNumber(randstate, 0, 1) ;

         randstate->update = RKMath_ARandomNumber(randstate, 0, 350) ;
            
         randstate->state_b = (unsigned)time(NULL) + RKMath_ARandomNumber(randstate, 0, 743276439) ;
            
        } else {
          
         randstate->update = RKMath_ARandomNumber(randstate, 0, 500) ;
            
         randstate->gate = RKMath_ARandomNumber(randstate, 0, 1) ;
          
         randstate->seconds = (unsigned)time(NULL) ;
            
         RKMath_ARandomNumber(randstate, 0, -1) ;
            
        }
    }
    
    randstate->state_b ^= (randstate->state_b << 21) ;
    
    randstate->state_b ^= (randstate->state_b >> 35) ;
    
    randstate->state_b ^= (randstate->state_b << 4) ;
    
    randval = (int) randstate->state_b % randmax ;
    
    randval = (randval < 0) ? -randval : randval ;
    
    randval = (randval < randmin) ? RKMath_AMoreRandomNumber(randstate, randmin, randmax) : randval ;
    
    return  randval ;
    
}

float RKMath_ARandomFloat( RKMath_RandState* randstate ) {
    
    return (((float)RKMath_ARandomNumber(randstate, 1, RAND_MAX)) / RAND_MAX) ;
}

float RKMath_AMoreRandomFloat( RKMath_RandState* randstate ) {
    
    return (((float)RKMath_AMoreRandomNumber(randstate, 1, RAND_MAX)) / RAND_MAX) ;
}

 float RKMath_Sum(float vec[], const int size) {
    
    int i = 0 ;
    
    float retval = 0 ;
    
    while ( i < size ) {
        
        retval = ( vec[i] + retval ) ;
        
        i++ ;
     }
    
    return retval ;
 }

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

//The Quake 3 InvSqrt

static float InvSqrt(float x) {
    
    float xhalf = 0.5f * x ;
    
    int i = *(int*)&x ;
    
    i = 0x5f3759df - (i>>1) ;
    
    x = *(float*)&i ;
    
    x = x * (1.5f - xhalf * x * x) ;
    
    return x ;
}

//Maybe, sometimes faster on some machines?

float RKMath_Sqrt( float x ) {
    
    return x * InvSqrt(x) ;
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

void RKMath_MinMax( float min_vec[], float max_vec[], float vec_a[], float vec_b[], const int size ) {
 
    int i = 0 ;
    
    while ( i < size ) {
    
        min_vec[i] = ( vec_a[i] < vec_b[i] ) ? vec_a[i] : vec_b[i] ;
        
        max_vec[i] = ( vec_a[i] > vec_b[i] ) ? vec_a[i] : vec_b[i] ;
        
        i++ ;
    }
    
}

void RKMath_MinMax_Solo(float* min, float* max, const float vec[], const int size) {
    
    int i = 0 ;
    
    *min = vec[i] ;
    
    *max = vec[i] ;
    
    while ( i < size ) {
        
        *min = ( vec[i] < *min ) ? vec[i] : *min  ;
        
        *max = ( vec[i] > *max ) ? vec[i] : *max ;
        
        i++ ;
    }
    
}

 void RKMath_Clamp(float vec[], const float min, const float max, const int size) {
    
    int i = 0 ;
    
    while ( i < size ) {
        
        vec[i] = ( vec[i] > max ) ? max : vec[i] ;
        
        vec[i] = ( vec[i] < min ) ? min : vec[i] ;
        
        i++ ;
    }
    
}

void RKMath_Matrix_Multiply( float outmatrix[], const float matrix_a[], const float matrix_b[] ) {
    
    int i = 0 ;
    
    int j = 0 ;
    
    int k = 0 ;
    
    int l = 0 ;
    
    int j_ = 0 ;
    
    int k_ = 0 ;
    
    while ( l < 4 ) {
        
        while ( k < (4*(l+1)) ) {
            
            k_ = ( (k+1) >= 4 ) ? ( k / 4 ) : k ;
            
            while ( j < (4*(k_+1)) ) {
                
                j_ = ( (j+1) >= 4 ) ? ( j / 4 ) : j ;
                
                while ( i < (4*(j_+1)) ) {
                    
                    outmatrix[k] = (matrix_a[i] * matrix_b[j]) + outmatrix[k] ;
                    
                    i++ ;
                }
                
                j++ ;
            }
            
            k++ ;
        }
        
        l++ ;
    }
    
}

void RKMath_Matrix_Point_Multiply( float outvec[], const float matrix[], const float point[] ) {
    
    outvec[RKM_X] = point[RKM_X] * matrix[0] + point[RKM_Y] * matrix[1] + point[RKM_Z] * matrix[2] + matrix[3] ;
    
    outvec[RKM_Y] = point[RKM_X] * matrix[4] + point[RKM_Y] * matrix[5] + point[RKM_Z] * matrix[6] + matrix[7] ;
    
    outvec[RKM_Z] = point[RKM_X] * matrix[8] + point[RKM_Y] * matrix[9] + point[RKM_Z] * matrix[10] + matrix[11] ;
    
    float w = point[RKM_X] * matrix[12] + point[RKM_Y] * matrix[13] + point[RKM_Z] * matrix[14] + matrix[15] ;
    
    RKMath_Vectorthat(w_vec, w) ;
    
    RKMath_Div(outvec, outvec, w_vec, 3) ;
}

void RKMath_Matrix_Vec_Multiply( float outvec[], const float matrix[], const float vec[] ) {
    
    outvec[RKM_X] = vec[RKM_X] * matrix[0] + vec[RKM_Y] * matrix[1] + vec[RKM_Z] * matrix[2] ;
    
    outvec[RKM_Y] = vec[RKM_X] * matrix[4] + vec[RKM_Y] * matrix[5] + vec[RKM_Z] * matrix[6] ;
    
    outvec[RKM_Z] = vec[RKM_X] * matrix[8] + vec[RKM_Y] * matrix[9] + vec[RKM_Z] * matrix[10]  ;
    
   }

