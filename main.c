#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

typedef struct node
{
    size_t size;
    struct node* next;
}header_t;

void* myMalloc(size_t size)
{
    if ( size < 1 ) 
    {
        printf("Requested size is less than 1 byte\n");
        return NULL;
    }

    size_t required_size = (size + PAGE_SIZE)/PAGE_SIZE * PAGE_SIZE; //Round-off to nearest PAGE_SIZE
    void* add = mmap(NULL, required_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    header_t* header = (header_t*)add;
    header->size = required_size;
    header->next = (void*)(0x01);

    return header;
}

void myFree(void* add)
{
   if ( add == NULL )
   {
       printf("Address passed to free is NULL");
       return;
   }

   header_t* header = (header_t*)add;
   if (header->next == (void*)(0x01))
   {
       size_t free_size = header->size;
       int ret = munmap(header, free_size);
       if (ret == -1)
       {
           printf("Error!!!! cannot free the memory Address %p\n", add);
           return;
       }

       printf("Successfully freed the memory Address %p\n", add);
   }
   return;
}

int main()
{
    size_t request_size = 10;
    void* add = myMalloc(request_size);
    if (add == NULL)
    {
        printf("Error allocating memory in myMalloc()\n");
        return -1;
    }
    printf("Address = %p\n", add);

    myFree(add);
    return 0;
}
