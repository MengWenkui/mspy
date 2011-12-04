#ifndef _MYUTIL_H
#define _MYUTIL_H

#define ERR_MSG_BUF 100

/* show error message to the stderr */
void myfatal(const char *message);

/*
 * wrappers for malloc and free,
 * if failed, print error message
 */
void *mymalloc(int size);
void *myrealloc(void *ptr, int size);
void myfree(void *p);

/* mylist utility data struct and API */
struct mylist_item {
    void *data;
};

typedef struct mylist_item mylist_item;
typedef void (*freefunc)(void *data);

struct mylist {
    int max;
    int size;
    mylist_item *item;
    freefunc free;
};

typedef struct mylist mylist;

mylist *mylist_create(int size, freefunc freeit);
int mylist_append(mylist *list, void *data);
int mylist_delete(mylist *list, int index);
void mylist_destroy(mylist *list);


#endif
