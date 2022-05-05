#ifndef HOMEMATH_H
#define HOMEMATH_H

// TODO(stylia): Make it so math.h is not needed
#include "math.h"

#define Prime(Value) Value##_Prime

inline real32 
SafeRatio0(r32 Value, r32 Divisor)
{
    Assert(Divisor != 0);
    real32 Result = Value / Divisor; 
    
    return(Result);
}

union v2
{
    struct
    {
        real32 x, y;
    };
    real32 E[2];
};

union v3
{
    struct
    {
        real32 x, y, z;
    };
    struct
    {
        v2 xy;
        real32 _Ignored00;
    };
    struct
    {
        real32 _Ignored01;
        v2 yz;
    };
    real32 E[3];
};

union v4
{
    struct
    {
        real32 x, y, z, w;
    };
    struct
    {
        real32 r, g, b, a;
    };
    struct
    {
        v2 xy;
        v2 zw;
    };
    struct
    {
        real32 _Ignored00;
        v2 yz;
        real32 _Ignored01;
    };
    struct
    {
        v3 xyz;
        real32 _Ignored02;
    };
    struct
    {
        real32 _Ignored03;
        v3 yzw;
    };
    real32 E[4];
};

inline v2 V2(real32 X, real32 Y)
{
    v2 Result;
    
    Result.x = X;
    Result.y = Y;
    
    return(Result);
}

inline v2 V2i(int32 X, int32 Y)
{
    v2 Result = {};
    
    Result.x = (real32)X;
    Result.y = (real32)Y;
    
    return(Result);
}

inline v2 V2i(uint32 X, uint32 Y)
{
    v2 Result = {};
    
    Result.x = (real32)X;
    Result.y = (real32)Y;
    
    return(Result);
}

inline v2 operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    
    return(Result);
}

inline v2 operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    
    return(Result);
}

inline v2 operator*(v2 A, real32 B)
{
    v2 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    
    return(Result);
}

inline v2 operator*(real32 A, v2 B)
{
    v2 Result;
    
    Result.x = A * B.x;
    Result.y = A * B.y;
    
    return(Result);
}

inline v2 operator/(v2 A, real32 B)
{
    v2 Result;
    
    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);
    
    return(Result);
}

inline v2 &operator+=(v2 &A, v2 B)
{
    A.x += B.x;
    A.y += B.y;
    
    return(A);
}

inline v3 V3(real32 X, real32 Y, real32 Z)
{
    v3 Result = {};
    
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    
    return(Result);
}

inline v3 V3i(int32 X, int32 Y, int32 Z)
{
    v3 Result = {};
    
    Result.x = (real32)X;
    Result.y = (real32)Y;
    Result.z = (real32)Z;
    
    return(Result);
}

inline v3 V3i(uint32 X, uint32 Y, uint32 Z)
{
    v3 Result = {};
    
    Result.x = (real32)X;
    Result.y = (real32)Y;
    Result.z = (real32)Z;
    
    return(Result);
}

inline v3 operator+(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return(Result);
}

inline v3 &operator+=(v3 &A, v3 B)
{
    A.x += B.x;
    A.y += B.y;
    A.z += B.z;
    
    return(A);
}

inline v3 operator-(v3 A, v3 B)
{
    v3 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return(Result);
}

inline v3 &operator-=(v3 &A, v3 B)
{
    A.x -= B.x;
    A.y -= B.y;
    A.z -= B.z;
    
    return(A);
}

inline v3 operator*(v3 A, real32 B)
{
    v3 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    
    return(Result);
}

inline v3 operator*(real32 A, v3 B)
{
    v3 Result;
    
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    
    return(Result);
}

inline v3 &operator*=(v3 &A, real32 B)
{
    A.x *= B;
    A.y *= B;
    A.z *= B;
    
    return(A);
}

inline v3 operator/(v3 A, real32 B)
{
    v3 Result;
    
    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);
    Result.z = SafeRatio0(A.z, B);
    
    return(Result);
}

inline v4 V4(real32 X, real32 Y, real32 Z, real32 W)
{
    v4 Result = {};
    
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;
    
    return(Result);
}

inline v4 V4i(int32 X, int32 Y, int32 Z, int32 W)
{
    v4 Result = {};
    
    Result.x = (real32)X;
    Result.y = (real32)Y;
    Result.z = (real32)Z;
    Result.w = (real32)W;
    
    return(Result);
}

inline v4 V4i(uint32 X, uint32 Y, uint32 Z, uint32 W)
{
    v4 Result = {};
    
    Result.x = (real32)X;
    Result.y = (real32)Y;
    Result.z = (real32)Z;
    Result.w = (real32)W;
    
    return(Result);
}

inline v4 &operator+=(v4 &A, v4 B)
{
    A.x += B.x;
    A.y += B.y;
    A.z += B.z;
    A.w += B.w;
    
    return(A);
}

inline real32 Inner(v2 A, v2 B)
{
    real32 Result = 0;
    
    Result = A.x*B.x + A.y*B.y;
    
    return(Result);
}

inline real32 Inner(v3 A, v3 B)
{
    real32 Result = 0;
    
    Result = A.x*B.x + A.y*B.y + A.z*B.z;
    
    return(Result);
}

inline real32 Square(real32 A)
{
    real32 Result = A*A;
    
    return(Result);
}

inline real32 Root(real32 A)
{
    real32 Result = sqrtf(A);
    
    return(Result);
}

struct rect2
{
    v2 Min;
    v2 Max;
};

struct rect3
{
    v3 Min;
    v3 Max;
};

inline bool32 
Intersect(rect2 A, rect2 B)
{
    bool32 Result = !((A.Max.x < B.Min.x) ||
                      (A.Max.y < B.Min.y) ||
                      (B.Max.x < A.Min.x) ||
                      (B.Max.y < A.Min.y));
    
    return(Result);
}

inline bool32 
Intersect(rect3 A, rect3 B)
{
    bool32 Result = !((A.Max.x < B.Min.x) ||
                      (A.Max.y < B.Min.y) ||
                      (A.Max.z < B.Min.z) ||
                      (B.Max.x < A.Min.x) ||
                      (B.Max.y < A.Min.y) ||
                      (B.Max.z < A.Min.z));
    
    return(Result);
}


inline rect2
RectCenterDim(v2 Center, real32 Dim)
{
    rect2 Result = {};
    
    Result.Min = {Center.x - Dim, Center.y - Dim};
    Result.Max = {Center.x + Dim, Center.y + Dim};
    
    return(Result);
}

inline rect2
RectCenterDim(v2 Center, v2 Dim)
{
    rect2 Result = {};
    
    Result.Min = Center - Dim;
    Result.Max = Center + Dim;
    
    return(Result);
}

inline rect2
RectCenterHalfDim(v2 Center, v2 Dim)
{
    rect2 Result = {};
    
    v2 HalfDim = 0.5f*Dim;
    
    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;
    
    return(Result);
}

inline rect3
RectCenterDim(v3 Center, real32 Dim)
{
    rect3 Result = {};
    
    Result.Min = {Center.x - Dim, Center.y - Dim, Center.z - Dim};
    Result.Max = {Center.x + Dim, Center.y + Dim, Center.z + Dim};
    
    return(Result);
}

inline rect3
RectCenterDim(v3 Center, v3 Dim)
{
    rect3 Result = {};
    
    Result.Min = Center - Dim;
    Result.Max = Center + Dim;
    
    return(Result);
}

inline rect3
RectCenterHalfDim(v3 Center, v3 Dim)
{
    rect3 Result = {};
    
    v3 HalfDim = 0.5f*Dim;
    
    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;
    
    return(Result);
}

inline i32 
LClampN(i32 Value, i32 N)
{
    i32 Result = (Value < N) ? (N) : (Value);
    
    return(Result);
}

inline i32 
LClamp0(i32 Value)
{
    i32 Result = LClampN(Value, 0);
    
    return(Result);
}

inline i32 
HClampN(i32 Value, i32 N)
{
    i32 Result = (Value > N) ? (N) : (Value);
    
    return(Result);
}



#endif //HOMEMATH_H
