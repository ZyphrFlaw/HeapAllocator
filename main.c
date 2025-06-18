#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/mman.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

typedef struct node
{
    size_t size;
    struct node* next;
}header_t;

header_t* free_list = NULL;

void* myMalloc(size_t size)
{
    if ( size < 1 ) 
    {
        printf("Requested size is less than 1 byte\n");
        return NULL;
    }

    size_t required_size = (size + PAGE_SIZE + sizeof(header_t) - 1)/PAGE_SIZE * PAGE_SIZE; //Round-off to nearest PAGE_SIZE
    printf("Requested size = %zu\n", required_size);
    header_t* prev = NULL;
    header_t* curr = free_list;
    while(curr != NULL)
    {
        printf("curr Size = %zu\n", curr->size);
        if (required_size <= curr->size)
        {
            printf("In realloc()\n");
            header_t* allocated_block = curr;
            header_t* new_block = (header_t*)((char*)curr + required_size);
            new_block->size = curr->size - required_size;
            new_block->next = curr->next;
            if (prev == NULL)
                free_list = new_block;
            else
                prev->next = new_block;
            return (void*)(allocated_block + 1);
        }
        prev = curr;
        curr = curr->next;
    }
    void* add = mmap(NULL, required_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ( add == MAP_FAILED )
    {
        printf("Error mmp() Failed to allocate memory");
        return NULL;
    }
    header_t* header = (header_t*)add;
    header->size = required_size;
    header->next = (void*)(0x01);
    return(void*)(header + 1);
}

void myFree(void* add)
{
   if ( add == NULL )
   {
       printf("Address passed to free is NULL");
       return;
   }

   header_t* header = (header_t*)add - 1;
   printf("find = %p\n", header->next);
   if (header->next == (void*)(0x01) && header->size > 4096509)
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
   else
   {
       if(free_list != NULL)
       {
            header_t* temp = free_list;
            free_list = (header_t*)(header);
            free_list->size = header->size;
            free_list->next = temp;
       }
       else
       {
            free_list = (header_t*)(header);
            free_list->size = header->size;
            free_list->next = NULL;
       }
   }
   return;
}

int main()
{
    size_t request_size = 10000;
    for (int i = 0;i<2;i++)
    {
        int* add = (int*)myMalloc(request_size);
        printf("New Memory Address = %p\n", add);
        if (add == NULL)
        {
            printf("Error allocating memory in myMalloc()\n");
            return -1;
        }
        myFree(add);
        request_size = 10;
    }
    while(free_list != NULL)
    {
        printf("Address = %p and Size = %zu\n", free_list, free_list->size);
        free_list = free_list->next;
    }
    return 0;
}
