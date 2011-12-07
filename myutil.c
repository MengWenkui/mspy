#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "myutil.h"

void myfatal(const char *message)
{
    assert(message);
    fprintf(stderr, "%s: %s\n",
            message, strerror(errno));
    exit(-1);
}

void *mymalloc(int size)
{
    assert(size >= 0);
    void *p = malloc(size);
    if(!p) {
        myfatal("out of memory");
    }
    return p;
}

void *myrealloc(void *ptr, int size)
{
    assert(size >= 0 && ptr);
    void *p = realloc(ptr, size);
    if(!p) {
        myfatal("out of memory");
    }
    return p;
}

void myfree(void *p)
{
    free(p);
}

mylist *mylist_create(int size, freefunc freeit)
{
    assert(size > 0);
    mylist *list = (mylist *)mymalloc(sizeof(mylist));
    list->max = size;
    list->size = 0;
    list->item = (mylist_item *)mymalloc(size * sizeof(mylist_item));
    list->free = freeit;
    return list;
}

int mylist_append(mylist *list, void *data)
{
    assert(list && data);
    if(list->size < list->max) {
        list->item[list->size].data = data;
        list->size++;
        return 0;
    }

    /* exapand the list first */
    list->max *= 2;
    list->item = (mylist_item *)myrealloc(list->item, list->max * sizeof(mylist_item));
    list->item[list->size].data = data;
    list->size++;
    return 0 ;
}

int mylist_delete(mylist *list, int index)
{
    assert(list && index >= 0 && index < list->size);
    for(int i = index; i < list->size - 1; i++) {
        list->item[i] = list->item[i+1];
    }
    list->size--;
    return 0;
}

void mylist_destroy(mylist *list)
{
    assert(list);
    if(list->free) {
        for(int i = 0; i < list->size; i++) {
            list->free(list->item[i].data);
        }
    }
    myfree(list->item);
    myfree(list);
}

