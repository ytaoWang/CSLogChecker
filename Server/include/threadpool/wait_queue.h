/*
 * wait_queue communication between thread
 */

#ifndef __WAIT_QUEUE_H_
#define __WAIT_QUEUE_H_

#include <semaphore.h>

#ifdef __cplusplus

extern "C" {

#endif

typedef struct 
{
    int semid;
    sem_t sem;
} wait_queue_t;



int wait_queue_init(wait_queue_t *w);

int wait_queue_up(wait_queue_t *w);
int wait_queue_up_nonblock(wait_queue_t *w);
    
int wait_queue_down(wait_queue_t *w);

int wait_queue_down_nonblock(wait_queue_t *w);
    
int wait_queue_size(wait_queue_t *w,int *val);    

int wait_queue_destroy(wait_queue_t *w);
    

#ifdef __cplusplus
}
#endif

#endif
