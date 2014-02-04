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
#include <memory.h>
#include <pthread.h>
#include <stdint.h>

typedef struct str
{
	volatile struct str *next;
	unsigned char flag;
	int v;
	int tid;
} str_t;

#define T_NUM		500
volatile static struct str *list;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

#define ALLOC( tmp, type )\
	tmp = malloc( sizeof( type ) );\
	memset( tmp, 0, sizeof( type ) );

#define TRAVERSE\
	for( ; list!=NULL ; list=list->next ) {\
		i++;\
	}\
	printf( "total: %d\n", i );

#define cmpxchg( ptr, _old, _new, fail_label ) { \
	volatile uint32_t *__ptr = (volatile uint32_t *)(ptr);   \
  	asm goto( "lock; cmpxchg %1,%0 \t\n"           \
    	"jnz %l[" #fail_label "] \t\n"               \
    	: /* empty */                                \
    	: "m" (*__ptr), "r" (_new), "a" (_old)       \
    	: "memory", "cc"                             \
    	: fail_label );                              \
}

void addItem( struct str *i ) {
        volatile struct str *oldHead;

again:
        oldHead = list;
        i->next = oldHead;
        cmpxchg( &list, oldHead, i, again );
}

void *test( void *data )
{
	struct str *tmp;
	int i;

	for( i = 0; i < 1000; i++ ) {
		ALLOC( tmp, str_t );
		tmp->v = i;
		tmp->tid = pthread_self();
#ifdef T_PT
		pthread_mutex_lock( &mut );
		tmp->next = list;
		list = tmp;
		pthread_mutex_unlock( &mut );
#elif T_AT
		addItem( tmp );
#else
		tmp->next = list;
		list = tmp;
#endif
	}

	return NULL;
}

int main()
{
	struct str *tmp;
	int i;	
	pthread_t tid[ T_NUM ];	
	pthread_attr_t attr;

	memset( &list, 0, sizeof( struct str ) );

	// head and tail
	for( i = 0; i < 2; i++ ) {
		ALLOC( tmp, str_t );
		
		if( list == NULL ) {
			tmp->v = 0xffff;
			list = tmp;
		} else {
			tmp->v = 0xffff;
			tmp->next = list;
			list = tmp;
		}
	}

	pthread_attr_init( &attr );
   	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

	for( i = 0; i < T_NUM; i++ ) {
		pthread_create( &tid[ i ], &attr, test, NULL );
	}

	for( i = 0; i < T_NUM; i++ ) {
		pthread_join( tid[ i ], NULL );
	}

	TRAVERSE;

	return 0;
}
