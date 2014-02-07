#ifndef TYPES_H_
#define TYPES_H_

// intel&co 
#ifndef IS_LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN 	1
#endif

#undef ARCH64
#undef ARCH32
#if _WIN64 || __x86_64__ || __ppc64__
#define	ARCH64	1
#else
#define ARCH32	1
#endif

typedef signed long		slong;
typedef unsigned long   	ulong;
typedef long long		int64;
typedef unsigned long long	uint64;
typedef int			int32;
typedef unsigned int		uint32;
typedef short			int16;
typedef unsigned short		uint16;
typedef signed char		int8;
typedef unsigned char		uint8;
typedef unsigned char		byte;

#undef size_t
typedef uint32 size_t;

// pointer and inline type
#if _WIN32 || _WIN64
#define INLINE _inline
#if _WIN64
typedef uint64 	ptr;
#else
typedef uint32 	ptr;
#endif
#elif __GNUC__
#define INLINE inline
#if __x86_64__ || __ppc64__
typedef uint64	ptr;
#else
typedef uint32	ptr;
#endif
#endif

// floats
#ifndef TYPEDEF_FLOAT32
#define TYPEDEF_FLOAT32
typedef float           float32;
#endif

#ifndef TYPEDEF_FLOAT64
#define TYPEDEF_FLOAT64
typedef double          float64;
#endif

#define  FLOAT64	1 
#if defined(FLOAT64)
/* default to double precision floating point */
typedef float64 float_t;
#else 
typedef float32 float_t;
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1  
#endif

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

#define SUCCESS  0

#define ARRAY_SIZE(a)    (sizeof(a)/sizeof(a[0]))
#define MAXINT_32  	0x7FFFFFFFL
#define MAX_OBJ_NAME	32

#endif /* TYPES_H_ */
