/*
 * Copyright (C) Free Mind Foundation 2013
 * www.fmf-base.org xorentor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "types.h"
#include "string.h"

#ifdef FMF_LIBSTR_DEBUG
#include <stdio.h>
void dbgmsg( const char *s ) {
	printf( "DEBUG: [%s]\n", s );
}
#endif

/*-----------------------------------------------------------------------------
 * memcpy:
 *  
 * CAUTION: do not use with overleaping buffers like: 
    char tab[] = "qwertyuiopasdfghjklzxcvbnm";
    char* ptr_src = tab;
    char* ptr_dst = tab + 1;
    size_t sz = 5;
 *-----------------------------------------------------------------------------*/
void *fmf_memcpy( void *dest, const void *src, size_t count ) 
{
#ifdef FMF_LIBSTR_memcpy_linuxkernel_nasm
   	/*-----------------------------------------------------------------------------
    	* 32bit nasm implementation of linux kernel memcpy, it compiles on windows as well
    	*-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcpy_linuxkernel_nasm" );
#endif
	//if( count < 4 )
	// only small for now
		return nasm_memcpy_s( dest, src, count );
	//else
	//	return nasm_memcpy_b( dest, src, count );
#elif FMF_LIBSTR_memcpy_prefetch
	/*-----------------------------------------------------------------------------
	 *  32bit nasm implementation of memcpy prefetch
	 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcpy_prefetch" );
#endif
	return nasm_memcpy_prefetch( dest, src, count );
#elif FMF_LIBSTR_memcpy_linuxkernel_att
   	/*-----------------------------------------------------------------------------
    	* inline AT&T linux kernel memcpy
    	*-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcpy_linuxkernel_att" );
#endif
	int32 d0, d1, d2;
	if( count < 4 ) {
		register uint32  dummy;
		__asm__ __volatile__(
  		"rep; movsb"
  		:"=&D"(dest), "=&S"(src), "=&c"(dummy)
  		:"0" (dest), "1" (src),"2" (count)
  		: "memory");
  	} else
    		__asm__ __volatile__(
    		"rep ; movsl\n\t"
    		"testb $2,%b4\n\t"
    		"je 1f\n\t"
    		"movsw\n"
    		"1:\ttestb $1,%b4\n\t"
    		"je 2f\n\t"
    		"movsb\n"
    		"2:"
    		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
    		:"0" (count/4), "q" (count),"1" ((long) dest),"2" ((long) src)
    		: "memory");

  	return (dest);
#elif FMF_LIBSTR_all_memcpy_compressed
	
	/*-----------------------------------------------------------------------------
	 *  compressed
	 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcpy_compressed" );
#endif
        int32 i = 0;
        unsigned char *d = (unsigned char *)dest, *s = (unsigned char *)src;
        for (i = count >> 3; i > 0; i--) {
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
        }

        if (count & 1 << 2) {
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
        }

        if (count & 1 << 1) {
                *d++ = *s++;
                *d++ = *s++;
        }

        if (count & 1)
                *d++ = *s++;
        return dest;
#elif FMF_LIBSTR_memcpy_sse2vector
	/*-----------------------------------------------------------------------------
	 *  INTEL SSE2 auto vectorization optimized by the icc, gcc from 2009 onwards
	 *
	 * http://software.intel.com/en-us/articles/memcpy-performance/
	 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcpy_sse2vector" );
#endif
    	char *s1 = dest;
    	const char *s2 = src;
    	for(; 0<count; --count)*s1++ = *s2++;
    	return dest;
#else
	/*-----------------------------------------------------------------------------
	 *  generic, this is almost as same as INTEL SSE2 av
	 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcpy_default" );
#endif
	while( count-- ) {
		((char *)dest)[count] = ((char *)src)[count];
	}
	return dest;
#endif
}

char *fmf_strchr( const char *string, int32 c )
{
#ifdef FMF_LIBSTR_strchr_x86_32
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strchr_x86_32" );
#endif
        int32 d0;
        char *res;
        asm volatile("movb %%al,%%ah\n"
                "1:\tlodsb\n\t"
                "cmpb %%ah,%%al\n\t"
                "je 2f\n\t"
                "testb %%al,%%al\n\t"
                "jne 1b\n\t"
                "movl $1,%1\n"
                "2:\tmovl %1,%0\n\t"
                "decl %0"
                : "=a" (res), "=&S" (d0)
                : "1" (string), "" (c)
                : "memory");
        return res;
#else
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strchr_default" );
#endif

	while ( *string ) {
		if ( *string == c ) {
			return ( char * )string;
		}
		string++;
	}
	return (char *)0;
#endif
}

int32 fmf_strcmp( const char *string1, const char *string2 ) {
#ifdef FMF_LIBSTR_strcmp_sse42_nasm
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcmp_sse42_nasm" );
#endif
	return strcmp_sse42( string1, string2 );
#elif FMF_LIBSTR_strcmp_x86_32
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcmp_x86_32" );
#endif
        int32 d0, d1;
        int32 res;
        asm volatile("1:\tlodsb\n\t"
                 "scasb\n\t"
                "jne 2f\n\t"
                "testb %%al,%%al\n\t"
                "jne 1b\n\t"
                "xorl %%eax,%%eax\n\t"
                "jmp 3f\n"
                "2:\tsbbl %%eax,%%eax\n\t"
                "orb $1,%%al\n"
                "3:"
                : "=a" (res), "=&S" (d0), "=&D" (d1)
                : "1" (string1), "2" (string2)
                : "memory");
        return res;
#else
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcmp_default" );
#endif
	while( *string1 == *string2 && *string1 && *string2 ) {
		string1++;
		string2++;
	}
	return *string1 - *string2;
#endif
}

size_t fmf_strlen( const char *string ) 
{
#ifdef FMF_LIBSTR_strlen_sse42_nasm
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strlen_sse42_nasm" );
#endif
	return nasm_strlen_pcmp( string );
#elif FMF_LIBSTR_strlen_sse42_att
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strlen_sse42_att" );
#endif
	size_t a;
        __asm__ __volatile__(   "movl $-16, %%eax;\
                movl %%ecx, %%edx;\
                pxor %%xmm0, %%xmm0;\
                1:;\
                addl $16, %%eax;\
                pcmpistri $8, (%%edx,%%eax), %%xmm0;\
                jnz 1b;\
                addl %%ecx, %%eax;\
                " : "=a"(a) : "c"(string) );
        return a;
#elif FMF_LIBSTR_strlen_x86_32
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strlen_x86_32" );
#endif
        int32 d0;
        size_t res;
        asm volatile("repne\n\t"
                "scasb"
                : "=c" (res), "=&D" (d0)
                : "1" (string), "a" (0), "" (0xffffffffu)
                : "memory");
        return ~res - 1;
#else
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strlen_default" );
#endif
	const char *s;
	s = string;
	while( *s ) {
		s++;
	}
	return s - string;
#endif
}

size_t fmf_strxfrm( char *dst, const char *src, size_t n )
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strxfrm_default" );
#endif
	size_t srclen, copysize;

	srclen = fmf_strlen( src );
	if( n != 0 ) {
		copysize = ( srclen < n ) ? srclen : n - 1;
		fmf_memcpy( dst, src, copysize );
		dst[ copysize ] = 0;
	}
	return ( srclen );
}

size_t fmf_strspn( const char *s, const char *accept )
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strspn_default" );
#endif
        const char *p;
        const char *a;
        size_t count = 0;

        for (p = s; *p != '\0'; ++p) {
                for (a = accept; *a != '\0'; ++a) {
                        if (*p == *a)
                                break;
                }
                if (*a == '\0')
                        return count;
                ++count;
        }
        return count;
}

#ifdef FMF_LIBSTR_x86_32_strstr
char *fmf_strstr(const char *cs, const char *ct)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strstr_x86_32" );
#endif
	int     d0, d1;
	register char *__res;
	__asm__ __volatile__(
        "movl %6,%%edi\n\t"
        "repne\n\t"
        "scasb\n\t"
        "notl %%ecx\n\t"
        "decl %%ecx\n\t"        /* NOTE! This also sets Z if searchstring='' */
        "movl %%ecx,%%edx\n"
        "1:\tmovl %6,%%edi\n\t"
        "movl %%esi,%%eax\n\t"
        "movl %%edx,%%ecx\n\t"
        "repe\n\t"
        "cmpsb\n\t"
        "je 2f\n\t"             /* also works for empty string, see above */
        "xchgl %%eax,%%esi\n\t"
        "incl %%esi\n\t"
        "cmpb $0,-1(%%eax)\n\t"
        "jne 1b\n\t"
        "xorl %%eax,%%eax\n\t"
        "2:"
        : "=a" (__res), "=&c" (d0), "=&S" (d1)
        : "" (0), "1" (0xffffffff), "2" (cs), "g" (ct)
        : "dx", "di");
	return __res;
}
#elif FMF_LIBSTR_s390_strstr
char *fmf_strstr(const char * s1,const char * s2)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strstr_s390" );
#endif

        int32 l1, l2;

        l2 = __strend(s2) - s2;
        if (!l2)
                return (char *) s1;
        l1 = __strend(s1) - s1;
        while (l1-- >= l2) {
                register unsigned long r2 asm("2") = (unsigned long) s1;
                register unsigned long r3 asm("3") = (unsigned long) l2;
                register unsigned long r4 asm("4") = (unsigned long) s2;
                register unsigned long r5 asm("5") = (unsigned long) l2;
                int16 cc;

                asm volatile ("0: clcle %1,%3,0\n"
                              "   jo    0b\n"
                              "   ipm   %0\n"
                              "   srl   %0,28"
                              : "=&d" (cc), "+a" (r2), "+a" (r3),
                                "+a" (r4), "+a" (r5) : : "cc" );
                if (!cc)
                        return (char *) s1;
                s1++;
        }
        return NULL;
}
#else
char *fmf_strstr( const char *string, const char *strCharSet ) {
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strstr_default" );
#endif
	while ( *string ) {
		int32 i;

		for ( i = 0 ; strCharSet[i] ; i++ ) {
			if ( string[i] != strCharSet[i] ) {
				break;
			}
		}
		if ( !strCharSet[i] ) {
			return (char *)string;
		}
		string++;
	}
	return (char *)0;
}
#endif

#ifdef FMF_LIBSTR_x86_32_memchr
void *fmf_memchr(const void *cs, int32 c, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memchr_x86_32" );
#endif
        int32 d0;
        void *res;
        if (!count)
                return NULL;
        asm volatile("repne\n\t"
                "scasb\n\t"
                "je 1f\n\t"
                "movl $1,%0\n"
                "1:\tdecl %0"
                : "=D" (res), "=&c" (d0)
                : "a" (c), "" (cs), "1" (count)
                : "memory");
        return res;
}
#else
void *fmf_memchr(const void *s, int32 c, size_t n)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memchr_default" );
#endif
        const unsigned char *p = (unsigned char *)s;
        while (n-- != 0) {
               if ((unsigned char)c == *p++) {
                       return (void *)(p - 1);
                }
        }
        return NULL;
}
#endif

#ifdef FMF_LIBSTR_x86_32_memcmp
int32 fmf_memcmp( const void *s1, const void *s2, size_t len )
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcmp_x86_32" );
#endif
        uint8 diff;
        asm("repe; cmpsb; setnz %0"
            : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
        return diff;
}

#else
int32 fmf_memcmp( const void *cs, const void *ct, size_t count )
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memcmp_default" );
#endif

        const unsigned char *su1, *su2;
        int8 res = 0;

        for (su1 = (unsigned char *)cs, su2 = (unsigned char *)ct; 0 < count; ++su1, ++su2, count--)
               if ((res = *su1 - *su2) != 0)
                        break;
        return res;
}
#endif

#ifdef FMF_LIBSTR_x86_32_memmove
void *fmf_memmove(void *dest, const void *src, size_t n)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memmove_x86_32" );
#endif
       int32 d0,d1,d2,d3,d4,d5;
       char *ret = dest;

       __asm__ __volatile__(
               /* Handle more 16 bytes in loop */
               "cmp $0x10, %0\n\t"
               "jb     1f\n\t"

               /* Decide forward/backward copy mode */
               "cmp %2, %1\n\t"
               "jb     2f\n\t"

               /*
                * movs instruction have many startup latency
                * so we handle small size by general register.
                */
               "cmp  $680, %0\n\t"
               "jb 3f\n\t"
               /*
                * movs instruction is only good for aligned case.
                */
               "mov %1, %3\n\t"
               "xor %2, %3\n\t"
               "and $0xff, %3\n\t"
               "jz 4f\n\t"
               "3:\n\t"
               "sub $0x10, %0\n\t"

               /*
                * We gobble 16 bytes forward in each loop.
                */
               "3:\n\t"
               "sub $0x10, %0\n\t"
               "mov 0*4(%1), %3\n\t"
               "mov 1*4(%1), %4\n\t"
               "mov  %3, 0*4(%2)\n\t"
               "mov  %4, 1*4(%2)\n\t"
               "mov 2*4(%1), %3\n\t"
               "mov 3*4(%1), %4\n\t"
               "mov  %3, 2*4(%2)\n\t"
               "mov  %4, 3*4(%2)\n\t"
               "lea  0x10(%1), %1\n\t"
               "lea  0x10(%2), %2\n\t"
               "jae 3b\n\t"
               "add $0x10, %0\n\t"
               "jmp 1f\n\t"

               /*
                * Handle data forward by movs.
                */
               ".p2align 4\n\t"
               "4:\n\t"
               "mov -4(%1, %0), %3\n\t"
               "lea -4(%2, %0), %4\n\t"
               "shr $2, %0\n\t"
               "rep movsl\n\t"
               "mov %3, (%4)\n\t"
               "jmp 11f\n\t"
               /*
                * Handle data backward by movs.
                */
               ".p2align 4\n\t"
               "6:\n\t"
               "mov (%1), %3\n\t"
               "mov %2, %4\n\t"
               "lea -4(%1, %0), %1\n\t"
               "lea -4(%2, %0), %2\n\t"
               "shr $2, %0\n\t"
               "std\n\t"
               "rep movsl\n\t"
               "mov %3,(%4)\n\t"
               "cld\n\t"
               "jmp 11f\n\t"

               /*
                * Start to prepare for backward copy.
                */
                ".p2align 4\n\t"
                "2:\n\t"
                "cmp  $680, %0\n\t"
                "jb 5f\n\t"
                "mov %1, %3\n\t"
                "xor %2, %3\n\t"
                "and $0xff, %3\n\t"
                "jz 6b\n\t"

                /*
                 * Calculate copy position to tail.
                 */
                "5:\n\t"
                "add %0, %1\n\t"
                "add %0, %2\n\t"
                "sub $0x10, %0\n\t"

                /*
                 * We gobble 16 bytes backward in each loop.
                 */
                "7:\n\t"
                "sub $0x10, %0\n\t"

                "mov -1*4(%1), %3\n\t"
                "mov -2*4(%1), %4\n\t"
                "mov  %3, -1*4(%2)\n\t"
                "mov  %4, -2*4(%2)\n\t"
                "mov -3*4(%1), %3\n\t"
                "mov -4*4(%1), %4\n\t"
                "mov  %3, -3*4(%2)\n\t"
                "mov  %4, -4*4(%2)\n\t"
                "lea  -0x10(%1), %1\n\t"
                "lea  -0x10(%2), %2\n\t"
                "jae 7b\n\t"
                /*
                 * Calculate copy position to head.
                 */
                "add $0x10, %0\n\t"
                "sub %0, %1\n\t"
                "sub %0, %2\n\t"
 
                /*
                 * Move data from 8 bytes to 15 bytes.
                 */
                ".p2align 4\n\t"
                "1:\n\t"
                "cmp $8, %0\n\t"
                "jb 8f\n\t"
                "mov 0*4(%1), %3\n\t"
                "mov 1*4(%1), %4\n\t"
                "mov -2*4(%1, %0), %5\n\t"
                "mov -1*4(%1, %0), %1\n\t"

                "mov  %3, 0*4(%2)\n\t"
                "mov  %4, 1*4(%2)\n\t"
                "mov  %5, -2*4(%2, %0)\n\t"
                "mov  %1, -1*4(%2, %0)\n\t"
                "jmp 11f\n\t"

                /*
                 * Move data from 4 bytes to 7 bytes.
                 */
                ".p2align 4\n\t"
                "8:\n\t"
                "cmp $4, %0\n\t"
                "jb 9f\n\t"
                "mov 0*4(%1), %3\n\t"
                "mov -1*4(%1, %0), %4\n\t"
                "mov  %3, 0*4(%2)\n\t"
                "mov  %4, -1*4(%2, %0)\n\t"
                "jmp 11f\n\t"

                /*
                 * Move data from 2 bytes to 3 bytes.
                */
                ".p2align 4\n\t"
                "9:\n\t"
                "cmp $2, %0\n\t"
                "jb 10f\n\t"
                "movw 0*2(%1), %%dx\n\t"
                "movw -1*2(%1, %0), %%bx\n\t"
                "movw %%dx, 0*2(%2)\n\t"
                "movw %%bx, -1*2(%2, %0)\n\t"
                "jmp 11f\n\t"

                /*
                 * Move data for 1 byte.
                */
                ".p2align 4\n\t"
                "10:\n\t"
                "cmp $1, %0\n\t"
                "jb 11f\n\t"
                "movb (%1), %%cl\n\t"
                "movb %%cl, (%2)\n\t"
                ".p2align 4\n\t"
                "11:"
                : "=&c" (d0), "=&S" (d1), "=&D" (d2),
                  "=r" (d3),"=r" (d4), "=r"(d5)
                :"" (n),
                 "1" (src),
                 "2" (dest)
                :"memory");

        return ret;
}
#else
void *fmf_memmove(void *dest, const void *src, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memmove_default" );
#endif

        char *tmp;
        const char *s;

        if (dest <= src) {
                tmp = (char *)dest;
                s = (char *)src;
                while (count--)
                        *tmp++ = *s++;
        } else {
                tmp = (char *)dest;
                tmp += count;
                s = (char *)src;
                s += count;
                while (count--)
                       *--tmp = *--s;
        }
        return dest;
}
#endif

#ifdef FMF_LIBSTR_x86_32_memset
void *fmf_memset(void *s, int32 c, size_t n)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memset_x86_32" );
#endif
        int32 i;
        char *ss = s;

        for (i = 0; i < n; i++)
                ss[i] = c;
        return s;
}
#else
void *fmf_memset(void *s, int32 c, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "memset_default" );
#endif
        char *xs = (char *)s;
        while (count--)
                *xs++ = c;
        return s;
}
#endif

#ifdef FMF_LIBSTR_x86_32_strcat
char *fmf_strcat(char *dest, const char *src)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcat_x86_32" );
#endif
        int32 d0, d1, d2, d3;
        asm volatile("repne\n\t"
                "scasb\n\t"
                "decl %1\n"
                "1:\tlodsb\n\t"
                "stosb\n\t"
                "testb %%al,%%al\n\t"
                "jne 1b"
                : "=&S" (d0), "=&D" (d1), "=&a" (d2), "=&c" (d3)
                : "" (src), "1" (dest), "2" (0), "3" (0xffffffffu) : "memory");
        return dest;
}
#else
char *fmf_strcat(char *dest, const char *src)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcat_default" );
#endif
        char *tmp = dest;
        while (*dest)
                dest++;
        while ((*dest++ = *src++) != '\0')
                ;
        return tmp;
}
#endif

size_t fmf_strcspn( const char *s, const char *reject )
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcspn_default" );
#endif
        const char *p;
        const char *r;
        size_t count = 0;

        for (p = s; *p != '\0'; ++p) {
                for (r = reject; *r != '\0'; ++r) {
                        if (*p == *r)
                                return count;
                }
                ++count;
        }
        return count;
}

#ifdef FMF_LIBSTR_x86_32_strncat
char *fmf_strncat(char *dest, const char *src, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strncat_x86_32" );
#endif
        int32 d0, d1, d2, d3;
        asm volatile("repne\n\t"
               "scasb\n\t"
               "decl %1\n\t"
               "movl %8,%3\n"
               "1:\tdecl %3\n\t"
               "js 2f\n\t"
               "lodsb\n\t"
               "stosb\n\t"
               "testb %%al,%%al\n\t"
               "jne 1b\n"
               "2:\txorl %2,%2\n\t"
               "stosb"
               : "=&S" (d0), "=&D" (d1), "=&a" (d2), "=&c" (d3)
               : "" (src), "1" (dest), "2" (0), "3" (0xffffffffu), "g" (count)
               : "memory");
        return dest;
}
#else
char *fmf_strncat(char *dest, const char *src, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strncat_default" );
#endif

        char *tmp = dest;
        if (count) {
                while (*dest)
                        dest++;
                while ((*dest++ = *src++) != 0) {
                        if (--count == 0) {
                                *dest = '\0';
                                break;
                        }
                }
        }
        return tmp;
}
#endif

char *fmf_strrchr(const char *s, int32 c)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strrchr_default" );
#endif
       const char *p = s + fmf_strlen(s);
       do {
           if (*p == (char)c)
               return (char *)p;
       } while (--p >= s);
       return NULL;
}

#ifdef FMF_LIBSTR_x86_32_strncmp
int32 fmf_strncmp(const char *cs, const char *ct, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strncmp_x86_32" );
#endif
        int32 res;
        int32 d0, d1, d2;
        asm volatile("1:\tdecl %3\n\t"
                "js 2f\n\t"
                "lodsb\n\t"
                "scasb\n\t"
                "jne 3f\n\t"
                "testb %%al,%%al\n\t"
                "jne 1b\n"
                "2:\txorl %%eax,%%eax\n\t"
                "jmp 4f\n"
                "3:\tsbbl %%eax,%%eax\n\t"
                "orb $1,%%al\n"
                "4:"
                : "=a" (res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
                : "1" (cs), "2" (ct), "3" (count)
                : "memory");
        return res;
}
#else
int32 fmf_strncmp(const char *cs, const char *ct, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strncmp_default" );
#endif
        unsigned char c1, c2;
        while (count) {
                c1 = *cs++;
                c2 = *ct++;
                if (c1 != c2)
                        return c1 < c2 ? -1 : 1;
                if (!c1)
                        break;
                count--;
        }
        return 0;
}
#endif

char *fmf_strtok_r(char *s, const char *delim, char **last)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strtok_r__default" );
#endif
	char *spanp, *tok;
	int c, sc;
	if (s == NULL && (s = *last) == NULL)
		return (NULL);
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = '\0';
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
}

#ifdef FMF_LIBSTR_x86_32_strcpy
char *fmf_strcpy(char *dest, const char *src)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcpy_x86_32" );
#endif
        int32 d0, d1, d2;
        asm volatile("1:\tlodsb\n\t"
                "stosb\n\t"
                "testb %%al,%%al\n\t"
                "jne 1b"
                : "=&S" (d0), "=&D" (d1), "=&a" (d2)
                : "" (src), "1" (dest) : "memory");
        return dest;
}
#else
char *fmf_strcpy( char *strDestination, const char *strSource ) 
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strcpy_default" );
#endif
	char *s;
	s = strDestination;
	while( *strSource ) 
	{
		*s++ = *strSource++;
	}
	*s = 0;
	return strDestination;
}
#endif

#ifdef FMF_LIBSTR_x86_32_strncpy
char *fmf_strncpy(char *dest, const char *src, size_t count)
{
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strlen_x86_32" );
#endif
        int32 d0, d1, d2, d3;
        asm volatile("1:\tdecl %2\n\t"
                "js 2f\n\t"
                "lodsb\n\t"
                "stosb\n\t"
                "testb %%al,%%al\n\t"
                "jne 1b\n\t"
                "rep\n\t"
                "stosb\n"
                "2:"
                : "=&S" (d0), "=&D" (d1), "=&c" (d2), "=&a" (d3)
                : "" (src), "1" (dest), "2" (count) : "memory");
        return dest;
}
#else
char *fmf_strncpy( char *strDest, const char *strSource, size_t count ) {
#ifdef FMF_LIBSTR_DEBUG
	dbgmsg( "strncpy_default" );
#endif
	char	*s;

	s = strDest;
	while ( *strSource && count ) {
		*s++ = *strSource++;
		count--;
	}
	while ( count-- ) {
		*s++ = 0;
	}
	return strDest;
}
#endif
