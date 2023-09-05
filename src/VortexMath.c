/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexMath.c
//VortexMath, a n-dimensional vector math library designed to take advantage of autovectorization.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <math.h>
#include <Vortex/VortexMem.h>
#include <Vortex/VortexMath.h>


vortex_ulong VortexMath_GetInt64MaxVal(void) {
  #ifdef _WIN32
    return ULLONG_MAX;
  #else
    return ULONG_MAX;
  #endif  
}

//ARandomNumber from:https://stackoverflow.com/questions/13213395/adjusting-xorshift-generator-to-return-a-number-within-a-maximum
 void VortexMath_SeedRandomState(VortexMath_RandState* randstate, vortex_int seed) {
     randstate->init = 1;
     randstate->state_a = (long)time(NULL) + seed;
 }

 vortex_int VortexMath_ARandomNumber(VortexMath_RandState* randstate, 
     vortex_int randmin, vortex_int randmax) {
    vortex_int randval = 0;
    vortex_int oldmax = randmax;
    if ( randmin < 0 ) randmin = 0;
    if ( randmax == -1 ) randstate->init = 0;
    randmax++;
    if ( randmax <= 0 ) randmax = 1;
    if (!randstate->init) {
        randstate->state_a = (long)time(NULL);
        randstate->init = 1 ;
    }
    randstate->state_a ^= (randstate->state_a << 21);
    randstate->state_a ^= (randstate->state_a >> 35);
    randstate->state_a ^= (randstate->state_a << 4);
    randval = (int) randstate->state_a % randmax;
    randval = (randval < 0) ? -randval : randval;
    randval = 
      (randval < randmin) ? VortexMath_ARandomNumber(randstate, randmin, oldmax) : randval;
    return randval;
}

//Is it more random?
vortex_int VortexMath_AMoreRandomNumber(VortexMath_RandState* randstate, 
      vortex_int randmin, vortex_int randmax) {
    vortex_int randval = 0;
    vortex_int oldmax = randmax;
    if ( randmin < 0 ) randmin = 0;
     randmax++;
    if ( randmax <= 0 ) randmax = 1;
    if (!randstate->init_) {
        randstate->init_ = 1;
        randstate->update = 10;
        randstate->seconds = (long)time(NULL);
        randstate->gate = VortexMath_ARandomNumber(randstate, 0, 1);
        randstate->state_b = 
          (long)time(NULL) + VortexMath_ARandomNumber(randstate, 0, 743276439);
    }
    if ( ((long)time(NULL) - randstate->seconds) >= randstate->update ) {
        if (randstate->gate) {
         randstate->seconds = (long)time(NULL);
         randstate->gate = VortexMath_ARandomNumber(randstate, 0, 1);
         randstate->update = VortexMath_ARandomNumber(randstate, 0, 350);
         randstate->state_b = 
           (long)time(NULL) + VortexMath_ARandomNumber(randstate, 0, 743276439);
        } else {
         randstate->update = VortexMath_ARandomNumber(randstate, 0, 500);
         randstate->gate = VortexMath_ARandomNumber(randstate, 0, 1);
         randstate->seconds = (long)time(NULL);
         VortexMath_ARandomNumber(randstate, 0, -1);
        }
    }
    randstate->state_b ^= (randstate->state_b << 21);
    randstate->state_b ^= (randstate->state_b >> 35);
    randstate->state_b ^= (randstate->state_b << 4);
    randval = (vortex_int) randstate->state_b % randmax;
    randval = (randval < 0) ? -randval : randval;
    randval = 
      (randval < randmin) ? VortexMath_AMoreRandomNumber(randstate, randmin, oldmax) : randval;
    return randval;
}

vortex_float VortexMath_ARandomFloat(VortexMath_RandState* randstate) {
    return (((vortex_float)VortexMath_ARandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX);
}

vortex_float VortexMath_AMoreRandomFloat(VortexMath_RandState* randstate) {
    return (((vortex_float)VortexMath_AMoreRandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX);
}

vortex_double VortexMath_ARandomDouble(VortexMath_RandState* randstate) {
    return (((vortex_double)VortexMath_ARandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX);
}

vortex_double VortexMath_AMoreRandomDouble(VortexMath_RandState* randstate) {
    return (((vortex_double)VortexMath_AMoreRandomNumber(randstate, 0, RAND_MAX)) / RAND_MAX);
}

 vortex_float VortexMath_Sum(vortex_float vec[], const vortex_int size) {
    vortex_int i = 0;
    vortex_float retval = 0;
    while ( i < size ) {
        retval = ( vec[i] + retval );
        i++;
     }
    return retval;
 }

 void VortexMath_Add(vortex_float outvec[], const vortex_float vec_a[],
      const vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        outvec[i] = vec_a[i] + vec_b[i];
        i++;
    }
}

 void VortexMath_Sub(vortex_float outvec[], const vortex_float vec_a[],
      const vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        outvec[i] = vec_a[i] - vec_b[i];
        i++;
    }
}

 void VortexMath_Mul(vortex_float outvec[], const vortex_float vec_a[],
      const vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        outvec[i] = vec_a[i] * vec_b[i];
        i++;
    }
}

 void VortexMath_Div(vortex_float outvec[], const vortex_float vec_a[],
      const vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        outvec[i] = vec_a[i] / vec_b[i];
        i++;
    }
}

void VortexMath_Equal(vortex_float vec_a[], const vortex_float vec_b[],
     const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        vec_a[i] = vec_b[i];
        i++;
    }
}

void VortexMath_Set_Vec_Equal_To_A_Const(vortex_float vec[], 
      const vortex_float const__, const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        vec[i] = const__;
        i++;
    }
}

vortex_int VortexMath_IsEqual(const vortex_float vec_a[], 
      const vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        if ( vec_a[i] != vec_b[i] ) return 0;
        i++;
    }
    return 1;
}

 void VortexMath_Neg(vortex_float vec[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        vec[i] = -vec[i];
        i++;
    }
}

 vortex_float VortexMath_Abs(const vortex_float vec[], const vortex_int size) {
    vortex_int i = 0;
    vortex_float retval = 0;
    while ( i < size ) {
        retval = powf(vec[i],2) + retval;
        i++;
    }
    return sqrtf(retval);
}

 vortex_float VortexMath_Dot(const vortex_float vec_a[], 
     const vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    vortex_float retval = 0;
    while ( i < size ) {
       retval = ( vec_a[i] * vec_b[i] ) + retval;
        i++;
    }
    return retval;
}

 void VortexMath_Cross(vortex_float outvec[], const vortex_float vec_a[],
      const vortex_float vec_b[]) {
    VortexMath_Vectorthis(outvec,
         vec_a[1] * vec_b[2] - vec_a[2] * vec_b[1],
          vec_a[2] * vec_b[0] - vec_a[0] * vec_b[2],
           vec_a[0] * vec_b[1] - vec_a[1] * vec_b[0]) ;
}

void VortexMath_Norm(vortex_float outvec[], vortex_float vec[], const vortex_int size) {
    VortexMath_NewVector(One_Div_By_ABS_Vec, size);
    const vortex_float abs = VortexMath_Abs(vec, size);
    if ( abs != 0 ) {
        VortexMath_Set_Vec_Equal_To_A_Const(One_Div_By_ABS_Vec, 1.0f / abs, size);
        VortexMath_Mul(outvec, vec, One_Div_By_ABS_Vec, size);
    } else {
        VortexMath_Equal(outvec, vec, size);
    }
}

void VortexMath_Rotation(vortex_float out_quaternion[], vortex_float x, vortex_float y,
     vortex_float z, vortex_float w) {
    VortexMath_NewVector(axis_angle, 4);
    axis_angle[0] = x;
    axis_angle[1] = y;
    axis_angle[2] = z;
    axis_angle[3] = w;    
    double radians = (axis_angle[3] / 180) * M_PI; 
    float value = sin(radians / 2);  
    VortexMath_Vectorthat(value_vec, value);        
    VortexMath_Mul(out_quaternion, value_vec, axis_angle, 3);    
    out_quaternion[3] = cos(radians / 2);
}

void VortexMath_MergeRotation(vortex_float out_quaternion[],
      const vortex_float qa[], const vortex_float qb[]) {
    out_quaternion[0] = qa[3] * qb[0] + qa[0] * qb[3] + qa[1] * qb[2] - qa[2] * qb[1];
    out_quaternion[1] = qa[3] * qb[1] + qa[1] * qb[3] + qa[2] * qb[0] - qa[0] * qb[2];
    out_quaternion[2] = qa[3] * qb[2] + qa[2] * qb[3] + qa[0] * qb[1] - qa[1] * qb[0];
    out_quaternion[3] = qa[3] * qb[3] - qa[0] * qb[0] - qa[1] * qb[1] - qa[2] * qb[2];
}


void VortexMath_GetRotationMatrixFromQuaternion(vortex_float matrix[], const vortex_float quaternion[]) {
    vortex_float x,y,z,w;
    x = quaternion[0];
    y = quaternion[1];
    z = quaternion[2];
    w = quaternion[3];
    matrix[0] = 1 - (2 * (y*y+z*z));
    matrix[1] = (2 * (x*y+z*w));
    matrix[2] = (2 * (x*z-y*w));
    matrix[3] = (2 * (x*y-z*w));
    matrix[4] = 1 - (2 * (x*x+z*z));
    matrix[5] = (2 * (y*z+x*w));
    matrix[6] = (2 * (x*z+y*w));
    matrix[7]= (2 * (y*z-x*w));
    matrix[8] = 1 - (2 * (x*x+y*y));
}

//Fast inverse square root(Quake III)
#define toInt(var) *(vortex_int*)&var
#define toFloat(var) *(vortex_float*)&var
static float Vortex_fast_inverse_square_root(vortex_float num) {
    vortex_int z = toInt(num);
    z = 0x5f3759df - (z>>1);
    vortex_float q = toFloat(z);
    q = q * ( 1.5f - ( (0.5f * num) * ( q * q ) ) );
    return q;
}

//Maybe, sometimes faster on some machines?
vortex_float VortexMath_Sqrt(vortex_float x) {
    return x * Vortex_fast_inverse_square_root(x);
}

 float VortexMath_Distance(const vortex_float vec_a[], const vortex_float vec_b[],
      const vortex_int size) {
    vortex_int i = 0;
    vortex_float retval = 0;
    while ( i < size ) {
        retval = powf((vec_b[i] - vec_a[i]),2) + retval;
        i++;
    }
    return sqrtf(retval);
}

void VortexMath_MinMax(vortex_float min_vec[], vortex_float max_vec[],
     vortex_float vec_a[], vortex_float vec_b[], const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        min_vec[i] = ( vec_a[i] < vec_b[i] ) ? vec_a[i] : vec_b[i];
        max_vec[i] = ( vec_a[i] > vec_b[i] ) ? vec_a[i] : vec_b[i];
        i++;
    }
}

void VortexMath_MinMax_Solo(vortex_float* min, vortex_float* max,
     const vortex_float vec[], const vortex_int size) {
    vortex_int i = 0;
    *min = vec[i];
    *max = vec[i];
    while ( i < size ) {
        *min = ( vec[i] < *min ) ? vec[i] : *min;
        *max = ( vec[i] > *max ) ? vec[i] : *max;
        i++;
    }
}

 void VortexMath_Clamp(vortex_float vec[], const vortex_float min,
      const vortex_float max, const vortex_int size) {
    vortex_int i = 0;
    while ( i < size ) {
        vec[i] = ( vec[i] > max ) ? max : vec[i];
        vec[i] = ( vec[i] < min ) ? min : vec[i];
        i++;
    }
}

#ifdef VORTEXMATH_ENABLE_ATOMICS

void VortexMath_AtomicInc(vortex_atomic_int* val) {
    atomic_fetch_add_explicit(val,1,memory_order_relaxed);
}

void VortexMath_AtomicDec(vortex_atomic_int* val)  {
    atomic_fetch_sub_explicit(val,1,memory_order_relaxed);
}

vortex_atomic_bool VortexMath_AtomicRWC(vortex_atomic_int* read_val, vortex_int write_val, vortex_int* compare_val) {
    return atomic_compare_exchange_strong(read_val, compare_val, write_val);
}

#endif
