#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <pthread.h>

typedef struct str
{
	struct str *next;
	unsigned char flag;
	int v;
	int tid;
} str_t;

struct str *list;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
#define T_NUM		500

#define ALLOC( tmp, type )\
	tmp = malloc( sizeof( type ) );\
	memset( tmp, 0, sizeof( type ) );

#define TRAVERSE\
	for( ; list!=NULL ; list=list->next ) {\
		i++;\
	}\
	printf( "total: %d\n", i );

/* Insertion is straightforward:
 * a new list cell is created and then introduced
 * using single CAS operation on the next field of the proposed predecessor.
 */
#ifndef __i386__
#define LL_ADD( list, node )\
	asm volatile("\
		0:\
		movq	(%%rbx), %%rax;\
		movq	%%rax, (%%rcx);\
		movq	(%%rbx), %%rax;\
		lock 	cmpxchg	%%rdx, (%%rbx);\
		jnz 	0b;"\
		: : "b"(&(list->next)), "c"(&(node->next)), "d"(node) );

#define LL_DEL( node )\
	asm volatile ("\
		0:;\
		movb	$1, (%%rbx);"\
		: : "b"(&(node->flag)) );
#else
#define LL_ADD( list, node )\
	asm volatile("\
		0:\
		movl	(%%ebx), %%eax;\
		jmp	2f;\
		1:\
		movl	(%%eax), %%eax;\
		2:\
		bt	$0, 4(%%eax);\
		jc	1b;\
		movl	%%eax, (%%ecx);\
		movl	(%%ebx), %%eax;\
		lock 	cmpxchg	%%edx, (%%ebx);\
		jnz 	0b;\
		3:\
		"\
		: : "b"(&(list->next)), "c"(&(node->next)), "d"(node) );

#define LL_DEL( node )\
	asm volatile ("\
		0:;\
		movb	$1, (%%ebx);"\
		: : "b"(&(node->flag)) );
#endif


void *test( void *data )
{
	struct str *tmp;
	int i;

	for( i = 0; i < 100; i++ ) {
		ALLOC( tmp, str_t );
		tmp->v = i;
		tmp->tid = pthread_self();
#ifdef T_PT
		pthread_mutex_lock( &mut );
		tmp->next = list;
		list = tmp;
		pthread_mutex_unlock( &mut );
#elif T_AT
		LL_ADD( list, tmp );
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
