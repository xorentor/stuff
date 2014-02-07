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
 * */
#include <stdio.h>
#include <malloc.h>

#include "types.h"
#include "string.h"

#define F_128	"128"
#define F_256	"256"
#define F_512	"512"
#define	F_1024	"1024"

typedef unsigned long long ticks;

static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

static inline char *readFile( const char *fname )
{
        FILE *file;
        char *buffer;
	size_t read;
	size_t lSize;

	file = fopen( fname, "r" );
	if( file == NULL )
		printf("File error");
	fseek( file, 0, SEEK_END );
	lSize = ftell( file );
	rewind( file );
	buffer = (char*) malloc( sizeof(char) * lSize );
	if( buffer == NULL ) {
		printf("Memory error");
		return NULL;
	}
	read = fread( buffer, 1, lSize, file );
	if( lSize != read )
		printf("read error");
	fclose( file );
	return buffer;
}

int main()
{
	char z0[2048];
	int q;
	ticks t1, t2;
	char *buf;
	int cnt = 0;

	/*-----------------------------------------------------------------------------
	 *  memcpy
	 *-----------------------------------------------------------------------------*/
#define M_MCPY( i )\
	t1 = rdtsc();\
	for( q = 0; q < 1024; q++ ) {\
		fmf_memcpy( z0, buf + q*i, i );\
	}\
	t2 = rdtsc();\
	printf( "ticks per %d bytes long strings: %lld\n", i, t2 - t1 );

	printf("memcpy:\n");
	buf = readFile( F_1024 );	
	
	M_MCPY( 1024 );
	M_MCPY( 512 );
	M_MCPY( 256 );
	M_MCPY( 128 );

	/*-----------------------------------------------------------------------------
	 *  strlen
	 *-----------------------------------------------------------------------------*/
	printf("strlen:\n");
#define M_STRLEN( f, i )\
	buf = readFile( f );\
	for( q = 0; q < 1024; q++ ) {\
		fmf_memset( z0, 0, sizeof( z0 ) );\
		fmf_memcpy( z0, buf + q*i, i );\
		t1 = rdtsc();\
		fmf_strlen( z0 );\
		t2 = rdtsc();\
		cnt += ( t2 - t1 );\
	}\
	printf( "ticks per %d bytes long strings: %d\n", i, cnt );\
	free( buf );

	M_STRLEN( F_1024, 1024 );
	M_STRLEN( F_512, 512 );
	M_STRLEN( F_256, 256 );
	M_STRLEN( F_128, 128 );

	/*-----------------------------------------------------------------------------
	 *  strcmp
	 *-----------------------------------------------------------------------------*/
	printf("strcmp:\n");
#define M_STRCMP( f, i )\
	buf = readFile( f );\
	cnt = 0;\
	for( q = 0; q < 1024; q++ ) {\
		fmf_memset( z0, 0, sizeof( z0 ) );\
		fmf_memcpy( z0, buf + q*i, i );\
		t1 = rdtsc();\
		fmf_strcmp( z0, z0 );\
		t2 = rdtsc();\
		cnt += ( t2 - t1 );\
	}\
	printf( "ticks per %d bytes long strings: %d\n", i, cnt );\
	free( buf );

	M_STRCMP( F_1024, 1024 );
	M_STRCMP( F_512, 512 );
	M_STRCMP( F_256, 256 );
	M_STRCMP( F_128, 128 );

	/*-----------------------------------------------------------------------------
	 *  strstr
	 *-----------------------------------------------------------------------------*/
	printf("strstr:\n");
#define M_STRSTR( f, i )\
	buf = readFile( f );\
	cnt = 0;\
	for( q = 0; q < 1024; q++ ) {\
		fmf_memset( z0, 0, sizeof( z0 ) );\
		fmf_memcpy( z0, buf + q*i, i );\
		t1 = rdtsc();\
		fmf_strstr( z0, z0 );\
		t2 = rdtsc();\
		cnt += ( t2 - t1 );\
	}\
	printf( "ticks per %d bytes long strings: %d\n", i, cnt );\
	free( buf );

	M_STRSTR( F_1024, 1024 ); 
	M_STRSTR( F_512, 512 ); 
	M_STRSTR( F_256, 256 ); 
	M_STRSTR( F_128, 128 ); 
	
	/*-----------------------------------------------------------------------------
	 *  memcmp
	 *-----------------------------------------------------------------------------*/
	printf("memcmp:\n");
#define M_MCMP( f, i )\
	buf = readFile( f );\
	cnt = 0;\
	for( q = 0; q < 1024; q++ ) {\
		fmf_memset( z0, 0, sizeof( z0 ) );\
		fmf_memcpy( z0, buf + q*i, i );\
		t1 = rdtsc();\
		fmf_memcmp( z0, z0, i );\
		t2 = rdtsc();\
		cnt += ( t2 - t1 );\
	}\
	printf( "ticks per %d bytes long strings: %d\n", i, cnt );\
	free( buf );

	M_MCMP( F_1024, 1024 );
	M_MCMP( F_512, 512 );
	M_MCMP( F_256, 256 );
	M_MCMP( F_128, 128 );

	return 0;
}

/*
	printf("fmf_memcpy: %s\n", z );
	printf("fmf_strlen: %d\n", fmf_strlen(y) );
	printf("strcmp: %d\n", strcmp( x, y ) );
	printf("memcmp: %d\n", memcmp( x, y, sizeof(x) ) );
	printf("strstr: %s\n", strstr( y, x ) );


	char *q = memchr( z, 'a', sizeof(z) );
	memmove( z, y, sizeof(y) );


  char str[80];
  strcpy (str,"these ");
  strcat (str,"strings ");
  strcat (str,"are ");
t
w
  strcat (str,"concatenated.");
  puts (str);


  char str1[] = "This is a sample string";
  char * pch;
  printf ("Looking for the 's' character in \"%s\"...\n",str1);
  pch=strchr(str1,'s');
  while (pch!=NULL)
  {
    printf ("found at %d\n",pch-str1+1);
    pch=strchr(pch+1,'s');
  }

  char str2[] = "fcba73";
  char keys[] = "1234567890";
  int i;
  i = strcspn (str2,keys);
  printf ("The first number in str is at position %d.\n",i+1);


  char strtext[] = "129th";
  char cset[] = "1234567890";

  i = strspn (strtext,cset);
  printf ("The initial number has %d digits.\n",i);

  char str5[20];
  char str6[20];
  strcpy (str5,"To be ");
  strcpy (str6,"or not to be");
  strncat (str5, str6, 6);
  puts (str5);

  char str7[] = "This is a sample string";
  char * pch7;
  pch7=strrchr(str7,'s');
  printf ("Last occurence of 's' found at %d \n",pch7-str7+1);

  char str9[][5] = { "R2D2" , "C3PO" , "R2A6" };
  int n1;
  puts ("Looking for R2 astromech droids...");
  for (n1=0 ; n1<3 ; n1++)
    if (strncmp (str9[n1],"R2xx",2) == 0)
    {
      printf ("found %s\n",str9[n1]);
    }

  char str11[]="Sample string";
  char str12[40];
  char str13[40];
  strcpy (str12,str11);
  strcpy (str13,"copy successful");
  printf ("str1: %s\nstr2: %s\nstr3: %s\n",str11,str12,str13);

  char str91[]= "To be or not to be";
  char str92[40];
  char str93[40];

  strncpy ( str92, str91, sizeof(str92) );

  strncpy ( str93, str92, 5 );
  str93[5] = '\0';  

  puts (str91);
  puts (str92);
  puts (str93);


char test[80], blah[80];
char *sep = "\\/:;=-";
char *word, *phrase, *brkt, *brkb;

strcpy(test, "This;is.a:test:of=the/string\\tokenizer-function.");

for (word = strtok_r(test, sep, &brkt);
     word;
     word = strtok_r(NULL, sep, &brkt))
{
    strcpy(blah, "blah:blat:blab:blag");


    for (phrase = strtok_r(blah, sep, &brkb);
         phrase;
         phrase = strtok_r(NULL, sep, &brkb))
    {
        printf("So far we’re at %s:%s\n", word, phrase);
    }
}

*/
