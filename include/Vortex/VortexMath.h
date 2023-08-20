/*
 Copyright (c) 2014-2023 Jacob Gordon. All rights reserved.

 Permission to redistribution and use this software in source and binary forms, with or without modification is hereby granted.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: VortexMath.h
//Header file for VortexMath.

#ifndef Vortex_VortexMath_h
#define Vortex_VortexMath_h

#define _USE_MATH_DEFINES
#include <math.h>

#define VORTEX_X 0

#define VORTEX_Y 1

#define VORTEX_Z 2

#define VORTEX_R 0

#define VORTEX_G 1

#define VORTEX_B 2

#define VORTEX_A 3

#define  VortexMath_VectorType(name) vortex_float name[] //use for function declarations
#define  VortexMath_VectorTypeConst(name) const vortex_float name[] //use for function declarations
#define  VortexMath_NewVectorMalloc(size) vortex_c_array(size,float)
#define  VortexMath_NewVector(name, size) vortex_float name[size]
#define  VortexMath_Vectorit(name, x, y, z) VortexMath_NewVector(name, 3) = {x,y,z}
#define  VortexMath_Vectoris(name,size,...) VortexMath_NewVector(name,size) = {__VA_ARGS__}
#define  VortexMath_Vectorthat(name, that) VortexMath_Vectorit(name,that,that,that)
#define  VortexMath_Vectorthis(oldvec, x, y, z)\
oldvec[0] = x;\
oldvec[1] = y;\
oldvec[2] = z
#define  VortexMath_VectorCopy(vec_a, vec_b)\
vec_a[0] = vec_b[0];\
vec_a[1] = vec_b[1];\
vec_a[2] = vec_b[2] 
#define vortex_math_max(a,b) (((a)>(b))?(a):(b))
#define vortex_math_min(a,b) (((a)<(b))?(a):(b))
#define vortex_math_abs(a) (((a)<0) ? -(a) : (a))
#define vortex_math_zsgn(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

 typedef vortex_float* vortex_vector;

 typedef struct { 
  vortex_int init;
  vortex_int init_;
  vortex_int gate;
  vortex_long state_a;
  vortex_long state_b;
  vortex_long update;
  vortex_long seconds;
 }  VortexMath_RandState;

 void VortexMath_SeedRandomState(VortexMath_RandState* randstate, vortex_int seed);
 vortex_int VortexMath_ARandomNumber(VortexMath_RandState* randstate, vortex_int randmin, vortex_int randmax);
 vortex_int VortexMath_AMoreRandomNumber(VortexMath_RandState* randstate, vortex_int randmin, vortex_int randmax);
 vortex_float VortexMath_ARandomFloat(VortexMath_RandState* randstate);
 vortex_float VortexMath_AMoreRandomFloat(VortexMath_RandState* randstate);
 vortex_double VortexMath_ARandomDouble(VortexMath_RandState* randstate);
 vortex_double VortexMath_AMoreRandomDouble(VortexMath_RandState* randstate);
 vortex_float VortexMath_Sum(vortex_float vec[], const vortex_int size);
 void VortexMath_Add(vortex_float outvec[], const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Sub(vortex_float outvec[], const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Mul(vortex_float outvec[], const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Div(vortex_float outvec[], const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Equal(vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Set_Vec_Equal_To_A_Const(vortex_float vec[], const vortex_float const__, const vortex_int size);
 vortex_int VortexMath_IsEqual(const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Neg(vortex_float vec[], const vortex_int size);
 vortex_float VortexMath_Abs(const vortex_float vec[], const vortex_int size);
 vortex_float VortexMath_Dot(const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_Cross(vortex_float outvec[], const vortex_float vec_a[], const vortex_float vec_b[]);
 void VortexMath_Norm(vortex_float outvec[], vortex_float vec[], const vortex_int size);
 void VortexMath_Rotation(vortex_float out_quaternion[], vortex_float x, vortex_float y, vortex_float z, vortex_float w);
 void VortexMath_MergeRotation(vortex_float out_quaternion[],const vortex_float qa[], const vortex_float qb[]);
 void VortexMath_GetRotationMatrixFromQuaternion(vortex_float matrix[], const vortex_float quaternion[]);
 vortex_float VortexMath_Sqrt(vortex_float x);
 vortex_float VortexMath_Distance(const vortex_float vec_a[], const vortex_float vec_b[], const vortex_int size);
 void VortexMath_MinMax(vortex_float min_vec[], vortex_float max_vec[], vortex_float vec_a[], vortex_float vec_b[], const vortex_int size);
 void VortexMath_MinMax_Solo(vortex_float* min, vortex_float* max, const vortex_float vec[], const vortex_int size);
 void VortexMath_Clamp(vortex_float vec[], const vortex_float min, const vortex_float max, const vortex_int size);

///Atomics///

#ifdef VORTEXMATH_ENABLE_ATOMICS

#include <stdatomic.h>

typedef atomic_int vortex_atomic_int;
typedef atomic_bool vortex_atomic_bool;

void VortexMath_AtomicInc(vortex_atomic_int* val);
void VortexMath_AtomicDec(vortex_atomic_int* val);
vortex_atomic_bool VortexMath_AtomicRWC(vortex_atomic_int* read_val, vortex_int write_val, vortex_int* compare_val);

#endif

#endif /* Vortex_VortexMath_h */
