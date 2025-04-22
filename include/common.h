#ifndef COMMON_H_
#define COMMON_H_

#include "include_asm.h"

typedef char s8;
typedef short s16;
typedef int s32;
typedef long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef float f32;

#define STATIC_ASSERT(x) int static_assert(int static_assert[(x)? 1 : (-1)])

#endif // COMMON_H_
