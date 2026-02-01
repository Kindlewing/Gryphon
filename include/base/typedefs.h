#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define STATIC_ASSERT(cond, name) typedef char static_assert_##name[(cond) ? 1 : -1]
#define true ((b8)1)
#define false ((b8)0)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef unsigned long long usize;
typedef signed long long isize;

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;

STATIC_ASSERT(sizeof(b8) == 1, b8_must_be_8_bits);
STATIC_ASSERT(sizeof(b16) == 2, b16_must_be_16_bits);
STATIC_ASSERT(sizeof(b32) == 4, b32_must_be_32_bits);
STATIC_ASSERT(sizeof(u8) == 1, u8_must_be_8_bits);
STATIC_ASSERT(sizeof(u16) == 2, u16_must_be_16_bits);
STATIC_ASSERT(sizeof(u32) == 4, u32_must_be_32_bits);
STATIC_ASSERT(sizeof(u64) == 8, u64_must_be_64_bits);

STATIC_ASSERT(sizeof(i8) == 1, i8_must_be_8_bits);
STATIC_ASSERT(sizeof(i16) == 2, i16_must_be_16_bits);
STATIC_ASSERT(sizeof(i32) == 4, i32_must_be_32_bits);
STATIC_ASSERT(sizeof(i64) == 8, i64_must_be_64_bits);

STATIC_ASSERT(sizeof(f32) == 4, f32_must_be_32_bit_float);
STATIC_ASSERT(sizeof(f64) == 8, f64_must_be_64_bit_float);

STATIC_ASSERT(sizeof(void *) == sizeof(usize), pointer_size_mismatch);

#endif // TYPEDEFS_H
