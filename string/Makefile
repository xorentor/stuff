CC=gcc
FLAGS=-m32 -I./include/ -I../typedefs/ -Wall -mtune=native #-DFMF_LIBSTR_DEBUG
DS=-D

# one at the time
#FMF_LIBSTR_memcpy_linuxkernel_nasm
#FMF_LIBSTR_memcpy_prefetch
#FMF_LIBSTR_memcpy_linuxkernel_att
#FMF_LIBSTR_all_memcpy_compressed
#FMF_LIBSTR_memcpy_sse2vector
STRFLAGS+=$(DS)FMF_LIBSTR_memcpy_linuxkernel_att

# one at the time
#FMF_LIBSTR_strlen_sse42_nasm
#FMF_LIBSTR_strlen_sse42_att
#FMF_LIBSTR_strlen_x86_32 # this one is slow
STRFLAGS+=$(DS)FMF_LIBSTR_strlen_sse42_att

#FMF_LIBSTR_strcmp_sse42_nasm # don't use this one
#FMF_LIBSTR_strcmp_x86_32
STRFLAGS+=$(DS)FMF_LIBSTR_strcmp_x86_32

#FMF_LIBSTR_x86_32_strstr  # sigsegv
#STRFLAGS+=$(DS)FMF_LIBSTR_x86_32_strstr

#FMF_LIBSTR_x86_32_memcmp
STRFLAGS+=$(DS)FMF_LIBSTR_x86_32_memcmp

#FMF_LIBSTR_x86_32_memchr
#MEMCHR=$(DS)FMF_LIBSTR_x86_32_memchr

#FMF_LIBSTR_x86_32_memmove
#MEMMOVE=$(DS)FMF_LIBSTR_x86_32_memmove

#FMF_LIBSTR_x86_32_memset
#MEMSET=$(DS)FMF_LIBSTR_x86_32_memset

#FMF_LIBSTR_x86_32_strcat
#STRCAT=$(DS)FMF_LIBSTR_x86_32_strcat

#FMF_LIBSTR_x86_32_strncat
#STRNCAT=$(DS)FMF_LIBSTR_x86_32_strncat

#FMF_LIBSTR_x86_32_strncmp
#STRNCMP=$(DS)FMF_LIBSTR_x86_32_strncmp

#FMF_LIBSTR_x86_32_strcpy
#STRCPY=$(DS)FMF_LIBSTR_x86_32_strcpy

#FMF_LIBSTR_x86_32_strncpy
#STRNCPY=$(DS)FMF_LIBSTR_x86_32_strncpy

#FMF_LIBSTR_strchr_x86_32
#STRCHR=$(DS)FMF_LIBSTR_x86_32_strchr

all:
	mkdir -p ./lib/
	mkdir -p ./build/
	nasm -f elf32 strlen.asm -o ./build/strlen.o
	nasm -f elf32 memcpy_prefetch.asm -o ./build/memcpy_prefetch.o
	nasm -f elf32 strcmp.asm -o ./build/strcmp.o
	$(CC) $(FLAGS) $(STRFLAGS) -c string.c -o ./build/string.o
	ar -cvq ./lib/libfmfstr.a ./build/*.o
clean:
	rm ./build/*.o ./lib/libfmfstr.a
