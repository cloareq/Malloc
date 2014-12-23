#include "malloc.h"


void	*base=NULL;

t_block	find_block(t_block *last, size_t size)
{
	t_block block;
	block = base;
	
	while (block && !(block->used && block->size >= size))
	  {
	    *last = block;
	    block = block->next;
	  }
	return (block);
}

t_block extend_heap(t_block last, size_t size)
{
  int i;
  t_block block;
  block = sbrk(0);
  i = (long int)sbrk(BLOCK_SIZE + size);
	
  if (i < 0)
    return (NULL);
  block->size = size;
  block->next = NULL;
  block->prev = last;
  block->ptr = block->data;
  if (last)
    last->next = block;
  block->used = 0;
  return (block);
}


void    split_block(t_block block, size_t size)
{
  t_block new_block;
  new_block = (t_block)(block->data + size);
  new_block->size = block->size - size - BLOCK_SIZE;
  new_block->next = block->next;
  new_block->prev = block;
  new_block->used = 1;
  new_block->ptr = new_block->data;
  block->size = size;
  block->next = new_block;
  if (new_block->next)
    new_block->next->prev = new_block;
}


void	*malloc(size_t size)
{
  t_block block;
  t_block prev;
  size_t s;
  s = align4(size);
  if (base)
    {
      prev = base;
      block = find_block(&prev, s);
      if (block)
	{
	  if ((block->size - s) >= (BLOCK_SIZE + 4))
	    split_block(block, s);
	  block->used = 0;
	}
      else
	{
	  block = extend_heap(prev, s);
	  if (!block)
	    return (NULL);
	}
    }
  else
    {
      block = extend_heap(NULL, s);
      if (!block)
	return NULL;
      base = block;
    }
  return (block->data);
}

void	*calloc(size_t num, size_t size)
{
  size_t i = 0;
  void* init = malloc(num*size + 1*size);
	
  while (i < (num+1)*size)
    {
      ((char *)init)[i] = 0;
      i++;
    }
  return init;
}

t_block fusion(t_block block)
{
  if (block->next && block->next->used)
    {
      block->size += BLOCK_SIZE +block->next->size;
      block->next = block->next->next;
      if (block->next)
	block->next->prev=block;
    }
  return block;
}

t_block get_block(void *ptr)
{
  char *tmp = ptr;
  tmp -= BLOCK_SIZE;
  ptr = tmp;
  return (ptr);
}

int	valid_addr(void *ptr)
{
  if (base)
    {
      if (ptr>base && ptr<sbrk(0))
	return (ptr == (get_block(ptr))->ptr);
    }
  return 0;
}

void	free(void *ptr)
{
  t_block block;
  if (valid_addr(ptr))
    {
      block=get_block(ptr);
      block->used = 1;
      if (block->prev && block->prev->used)
	block=fusion(block->prev);
      if(block->next)
	fusion(block);
      else
	{
	  if(block->prev)
	    block->prev->next = NULL;
	  else
	    base = NULL;
	  brk(block);
	}
    }
}

void copy_block(t_block src, t_block dest)
{
  int *srcdata;
  int *destdata;
  size_t i = 0;
  srcdata = src->ptr;
  destdata = dest->ptr;
  while ((i * 4) < src->size && (i * 4) < dest->size)
    {
      destdata[i] = srcdata[i];
      i++;
    }
}

void *realloc(void *ptr, size_t size)
{
  size_t s;
  t_block block;
  t_block new;
  void *newptr;

  if (!ptr)
    return (malloc(size));
  if (valid_addr(ptr))
    {
      s = align4(size);
      block = get_block(ptr);
      if (block->size >= s)
	{
	  if (block->size - s >= (BLOCK_SIZE + 4))
	    split_block(block,s);
	}
      else
	{
	  if (block->next && block->next->used && (block->size + BLOCK_SIZE + block->next->size) >= s)
	    {
	      fusion(block);
	      if (block->size - s >= (BLOCK_SIZE + 4))
		split_block(block,s);
	    }
	  else
	    {
	      newptr = malloc(s);
	      if (!newptr)
		return (NULL);
	      new = get_block(newptr);
	      copy_block(block,new);
	      free(ptr);
	      return (newptr);
            }
        }
      return (ptr);
    }
  return (NULL);
}

void	show_alloc_mem()
{
  t_block block;
  printf("break : %p\n", sbrk(0));
  while (block != NULL)
    {
      printf("%p - %p : %u octets\n", block, (block+block->size), (unsigned int)(block->size));
      block = block->next;
    }
}
