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
#ifndef FMF_STRING_H_
#define	FMF_STRING_H_	1

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 *  memcpy
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_memcpy_linuxkernel_nasm
extern void 	*nasm_memcpy_s( void *, const void *, size_t );
//extern void 	*nasm_memcpy_b( void *, const void *, size_t );
#elif FMF_LIBSTR_memcpy_prefetch
extern void	*nasm_memcpy_prefetch( void *, const void *, size_t );
#elif FMF_LIBSTR_memcpy_linuxkernel_att
#elif FMF_LIBSTR_all_memcpy_compressed
#elif FMF_LIBSTR_memcpy_sse2vector
#else
#endif
void    *fmf_memcpy(void *, const void *, size_t);

/*-----------------------------------------------------------------------------
 *  strlen
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_strlen_sse42_nasm
extern size_t  	nasm_strlen_pcmp( const char * );
#elif FMF_LIBSTR_strlen_sse42_att
#elif FMF_LIBSTR_strlen_x86_32
#else
#endif
size_t   fmf_strlen(const char *);

/*-----------------------------------------------------------------------------
 *  strcmp
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_strcmp_sse42_nasm
extern int32	strcmp_sse42(const char *, const char *);
#elif FMF_LIBSTR_strcmp_x86_32
#else
#endif
int32    fmf_strcmp(const char *, const char *);

/*-----------------------------------------------------------------------------
 * strstr
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_strstr
#elif FMF_LIBSTR_s390_strstr
#else
#endif
char    *fmf_strstr(const char *, const char *);

/*-----------------------------------------------------------------------------
 *  memcmp
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_memcmp
#else
#endif
int32   fmf_memcmp(const void *, const void *, size_t);

/*-----------------------------------------------------------------------------
 *  memchr 
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_memchr
#else
#endif
void    *fmf_memchr(const void *, int32, size_t);

/*-----------------------------------------------------------------------------
 *  memmove
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_memmove
#else
#endif
void    *fmf_memmove(void *, const void *, size_t);

/*-----------------------------------------------------------------------------
 *  memset
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_memset
#else
#endif
void    *fmf_memset(void *, int32, size_t);

/*-----------------------------------------------------------------------------
 *  strcat
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_strcat
#else
#endif
char    *fmf_strcat(char *, const char *);

/*-----------------------------------------------------------------------------
 *  strxfrm
 *-----------------------------------------------------------------------------*/
size_t   fmf_strxfrm(char *, const char *, size_t);

/*-----------------------------------------------------------------------------
 *  strchr
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_strchr_x86_32
#else
#endif
char    *fmf_strchr(const char *, int32);

/*-----------------------------------------------------------------------------
 *  strcspn
 *-----------------------------------------------------------------------------*/
size_t   fmf_strcspn(const char *, const char *);

/*-----------------------------------------------------------------------------
 *  strspn
 *-----------------------------------------------------------------------------*/
size_t   fmf_strspn(const char *, const char *);


/*-----------------------------------------------------------------------------
 *  strncat
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_strncat
#else
#endif
char    *fmf_strncat(char *, const char *, size_t);

/*-----------------------------------------------------------------------------
 *  strrchr
 *-----------------------------------------------------------------------------*/
char    *fmf_strrchr(const char *, int32);

/*-----------------------------------------------------------------------------
 *  strncmp
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_strncmp
#else
#endif
int32    fmf_strncmp(const char *, const char *, size_t);

/*-----------------------------------------------------------------------------
 *  strtok_r
 *-----------------------------------------------------------------------------*/
char 	*fmf_strtok_r(char *, const char *, char **);

/*-----------------------------------------------------------------------------
 *  strcpy
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_strcpy
#else
#endif
char    *fmf_strcpy(char *, const char *);

/*-----------------------------------------------------------------------------
 *  strncpy
 *-----------------------------------------------------------------------------*/
#ifdef FMF_LIBSTR_x86_32_strncpy
#else
#endif
char    *fmf_strncpy(char *, const char *, size_t);

/*
 * we don't support some non-sandards:
 *
 * void    *memccpy(void *, const void *, int32, size_t);
 * char    *strdup(const char *);
 * char    *strerror(int32);
 * int32    strcoll(const char *, const char *);
 * strtok - not thread safe, use strtok_r instead
 */

#ifdef __cplusplus
}
#endif

#endif
