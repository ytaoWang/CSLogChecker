#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "wait_queue.h"

static int __wait_queue_up(wait_queue_t *w,int block);
static int __wait_queue_down(wait_queue_t *w,int block);


int wait_queue_init(wait_queue_t *w)
{
    return sem_init(&w->sem,0,0);
}


static int __wait_queue_up(wait_queue_t *w,int block)
{
    return sem_post(&w->sem);
}

static int __wait_queue_down(wait_queue_t *w,int block)
{
    int ret;
    
    if(block)
        return sem_wait(&w->sem);
    
    if((ret = sem_trywait(&w->sem)) < 0)
    {    
        if(errno == EAGAIN) return 0;
        return -1;
    }

    return 0;
}

int wait_queue_up(wait_queue_t *w)
{
    return __wait_queue_up(w,1);
}

int wait_queue_up_nonblock(wait_queue_t *w)
{
    return __wait_queue_up(w,0);
}

int wait_queue_down(wait_queue_t *w)
{
    return __wait_queue_down(w,1);
}

int wait_queue_down_nonblock(wait_queue_t *w)
{
    return __wait_queue_down(w,0);
}

int wait_queue_size(wait_queue_t *w,int *val)
{
    return sem_getvalue(&w->sem,val);
}


int wait_queue_destroy(wait_queue_t *w)
{
    return sem_destroy(&w->sem);
}
