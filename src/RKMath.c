/*
 Copyright (c) 2014-2018 Jacob Gordon. All rights reserved.
 
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

#include <time.h>
#include <math.h>
#include <RKLib/RKMem.h>
#include <RKLib/RKMath.h>

//ARandomNumber from:https://stackoverflow.com/questions/13213395/adjusting-xorshift-generator-to-return-a-number-within-a-maximum

 void RKMath_SeedRandomState( RKMath_RandState* randstate, int seed ) {
    
     randstate->init = 1 ;
     
     randstate->state_a = (long)time(NULL) + seed ;
 }

 int RKMath_ARandomNumber( RKMath_RandState* randstate, int randmin, int randmax ) {
    
    int randval = 0 ;
    
    int oldmax = randmax ;
     
    if ( randmin < 0 ) randmin = 0 ;
     
    if ( randmax == -1 ) randstate->init = 0 ;
    
    randmax++ ;
    
    if ( randmax <= 0 ) randmax = 1 ;
    
    if (!randstate->init) {
        
        randstate->state_a = (long)time(NULL) ;
        
        randstate->init = 1 ;
    }
    
    randstate->state_a ^= (randstate->state_a << 21) ;
    
    randstate->state_a ^= (randstate->state_a >> 35) ;
    
    randstate->state_a ^= (randstate->state_a << 4) ;
    
    randval = (int) randstate->state_a % randmax ;
    
    randval = (randval < 0) ? -randval : randval ;
    
    randval = (randval < randmin) ? RKMath_ARandomNumber(randstate, randmin, oldmax) : randval ;
    
    return randval ;
    
}

//Is it more random?

int RKMath_AMoreRandomNumber( RKMath_RandState* randstate, int randmin, int randmax ) {
    
    int randval = 0 ;
    
    int oldmax = randmax ;
    
    if ( randmin < 0 ) randmin = 0 ;
    
     randmax++ ;
    
    if ( randmax <= 0 ) randmax = 1 ;
    
    if (!randstate->init_) {
        
        randstate->init_ = 1 ;
        
        randstate->update = 10 ;
        
        randstate->seconds = (long)time(NULL) ;
        
        randstate->gate = RKMath_ARandomNumber(randstate, 0, 1) ;
        
        randstate->state_b = (long)time(NULL) + RKMath_ARandomNumber(randstate, 0, 743276439) ;
    }
    
    if ( ((long)time(NULL) - randstate->seconds) >= randstate->update ) {
        
        if (randstate->gate) {
            
         randstate->seconds = (long)time(NULL) ;
        
         randstate->gate = RKMath_ARandomNumber(randstate, 0, 1) ;

         randstate->update = RKMath_ARandomNumber(randstate, 0, 350) ;
            
         randstate->state_b = (long)time(NULL) + RKMath_ARandomNumber(randstate, 0, 743276439) ;
            
        } else {
          
         randstate->update = RKMath_ARandomNumber(randstate, 0, 500) ;
            
         randstate->gate = RKMath_ARandomNumber(randstate, 0, 1) ;
          
         randstate->seconds = (long)time(NULL) ;
            
         RKMath_ARandomNumber(randstate, 0, -1) ;
            
        }
    }
    
    randstate->state_b ^= (randstate->state_b << 21) ;
    
    randstate->state_b ^= (randstate->state_b >> 35) ;
    
    randstate->state_b ^= (randstate->state_b << 4) ;
    
    randval = (int) randstate->state_b % randmax ;
    
    randval = (randval < 0) ? -randval : randval ;
    
    randval = (randval < randmin) ? RKMath_AMoreRandomNumber(randstate, randmin, oldmax) : randval ;
    
    return randval ;

}

float RKMath_ARandomFloat( RKMath_RandState* randstate ) {
    
    return (((float)RKMath_ARandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX) ;
}

float RKMath_AMoreRandomFloat( RKMath_RandState* randstate ) {
    
    return (((float)RKMath_AMoreRandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX) ;
}

double RKMath_ARandomDouble( RKMath_RandState* randstate ) {
    
    return (((double)RKMath_ARandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX) ;
}

double RKMath_AMoreRandomDouble( RKMath_RandState* randstate ) {
    
    return (((double)RKMath_AMoreRandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX) ;
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
//https://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/

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

#ifdef RKMATH_ENABLE_ATOMICS

void RKMath_AtomicInc( RKMAtomicInt* val ) {
    
    atomic_fetch_add_explicit(val,1,memory_order_relaxed) ;
}

void RKMath_AtomicDec( RKMAtomicInt* val )  {
    
    atomic_fetch_sub_explicit(val,1,memory_order_relaxed) ;
}

RKMAtomicBool RKMath_AtomicRWC( RKMAtomicInt* read_val, int write_val, int* compare_val ) {
    
    return atomic_compare_exchange_strong(read_val, compare_val, write_val) ;
}

#endif