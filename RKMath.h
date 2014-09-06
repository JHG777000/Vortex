/*
 Copyright (c) 2014 Jacob Gordon. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//File: RKMath.h
//Header file for RKMath.

#ifndef MBRT_RKMath_h
#define MBRT_RKMath_h

#define RKM_X 0

#define RKM_Y 1

#define RKM_Z 2

#define RKM_R 0

#define RKM_B 1

#define RKM_G 2

#define  RKMath_VectorType(name) float name[] //use for function declarations

#define  RKMath_VectorTypeConst(name) const float name[] //use for function declarations

#define  RKMath_NewVectorMalloc(size) RKMem_CArray(size,float)

#define  RKMath_NewVector(name, size) float name[size]

#define  RKMath_Vectorit(name, x, y, z) RKMath_NewVector(name, 3) = {x,y,z}

#define  RKMath_Vectorthat(name, that) RKMath_Vectorit(name,that,that,that)

#define  RKMath_Vectorthis(oldvec, x, y, z) \
oldvec[0] = x ; \
oldvec[1] = y ; \
oldvec[2] = z  \

#define  RKMath_VectorCopy(vec_a, vec_b) \
vec_a[0] = vec_b[0] ; \
vec_a[1] = vec_b[1] ; \
vec_a[2] = vec_b[2]  \


 typedef float* RKMVector ;

 void RKMath_Add(float outvec[], const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Sub(float outvec[], const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Mul(float outvec[], const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Div(float outvec[], const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Equal(float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Set_Vec_Equal_To_A_Const(float vec[], const float const__, const int size) ;

 int RKMath_IsEqual(const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Neg(float vec[], const int size) ;

 float RKMath_Abs(const float vec[], const int size) ;

 float RKMath_Dot(const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Cross(float outvec[], const float vec_a[], const float vec_b[] ) ;

 void RKMath_Norm(float outvec[], float vec[], const int size) ;

 float RKMath_Distance(const float vec_a[], const float vec_b[], const int size) ;

 void RKMath_Clamp(float vec[], const float min, const float max, const int size) ;

#endif
