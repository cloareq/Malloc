#ifndef NEWMALLOC_H__
#define NEWMALLOC_H__

#include <stdio.h>
#include <unistd.h>

typedef struct s_block
{
	size_t size;
	struct s_block *next;
	struct s_block *prev;
	int used;
	char data[1];
	void *ptr;
} *t_block;

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define BLOCK_SIZE sizeof(struct s_block)

#endif