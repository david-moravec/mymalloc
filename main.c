#include <basetsd.h>
#include <stddef.h>
#include <stdio.h>
#include <windows.h>

#define align8(x) (((((x) - 1) >> 3) << 3 ) + 8)
#define BLOCK_SIZE (sizeof(struct s_block))

void* base = NULL;

typedef struct s_block* t_block;

struct s_block {
  size_t size;
  t_block next;
  int free;
  char data[1];
};

t_block findBlock(t_block* last, size_t size) {
  t_block b = base;

  while (b && !(b->free && b->size >= size)) {
    *last = b;
  }

  return b;
}

void splitBlock(t_block b, size_t size) {
  t_block new;
  new = (void*)(b->data + size);
  new->size = b->size - size - BLOCK_SIZE;
  b->size = size;
  new->next = b->next;
  b->next = new;
  new->free = 1;
}

t_block extendHeap(t_block last, size_t size) {
  t_block p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + BLOCK_SIZE);
  if (p == NULL) {
    return NULL;
  }
  p->size = size;
  p->next = NULL;
  if (last != NULL) {
    last->next = p;
  }
  p->free = 0;
  return p;
}

void* mymalloc(size_t size) {
  t_block b, last;
  size_t s;
  s = align8(size);
  if (base == NULL) {
    b = extendHeap(NULL, s); 
    if (b == NULL) {
      base = b;
    }
  } else {
    last = base;
    b = findBlock(&last, s);
    if (b) {
      if ((b->size - s) >= (BLOCK_SIZE + 8)) {
        splitBlock(b, s);
      }
      b->free = 0;
    } else {
      b = extendHeap(last,s);
      if (b == NULL) {
        return NULL;
      }
    }
  } 

  printf("b:    %p\n", b);
  // printf("size: %llu\n", b->size);
  // printf("next: %p\n", &b->next);
  // printf("free: %p\n", &b->free);
  // printf("data: %p\n", b->data);
  // printf("sizeof: %llu\n", sizeof(struct s_block));
  return b->data;
}


struct test {
  size_t size;
  t_block next;
  int free;
};

int main() {
  int* test1 = (int*)mymalloc(96 * sizeof(int));
  // printf("%p\n", (void*)test1);
  // printf("%p\n", (void*)test1);
  // printf("size: %llu\n", test_t_block->size);
  // printf("%i\n", p->free);
  // printf("%i\n", p->next == NULL);

  int* test2 = (int*)mymalloc(24 * sizeof(int));
  test2[0] = 4;

  char* p = (char*)test1;
  t_block a = (t_block)(p - 20);

  printf("%p\n", a);
  printf("%p\n", a->next);
  return 0;
}
