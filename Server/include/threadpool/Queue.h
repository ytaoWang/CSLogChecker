#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <pthread.h>
#include <cstdlib>
#include <iostream>

#include "wait_queue.h"

template <typename T>
class Queue {
    struct Node 
    {
        struct Node *next;
        T value;
        Node():next(NULL)
        {}
    };
    
    struct Node *head;
    struct Node *tail;
    pthread_mutex_t hlock;
    pthread_mutex_t tlock;
    struct Node s;//satellite node
    unsigned int size;
    wait_queue_t wait;
    
public:
    Queue():head(&s),tail(&s),size(0)
    {
        //hlock(PTHREAD_MUTEX_INITIALIZER),tlock(PTHREAD_MUTEX_INITIALIZER)
        hlock = PTHREAD_MUTEX_INITIALIZER;
        tlock = PTHREAD_MUTEX_INITIALIZER;
        wait_queue_init(&wait);
    }
    
    int enQueue(const T & elem)
    {
        struct Node *pNode = new struct Node;
        
        pNode->value = elem;
        
        pthread_mutex_lock(&tlock);
        
        tail->next = pNode;
        tail = pNode;
        
        pthread_mutex_unlock(&tlock);
        
        /*
        #ifdef DEBUG
        std::cout << "a element has put into queue." << std::endl;
        #endif
        */
        wait_queue_up_nonblock(&wait);
        __sync_fetch_and_add(&size,1);
        
        return 0;
        
    }
    
    int deQueue(T *t)
    {
        struct Node *h1,*h2;
        int size;
        
        if(!t) return -1;
        
    __again: 
        pthread_mutex_lock(&hlock);

        h1 = head;
        h2 = h1->next;
        if(!h2) {
            pthread_mutex_unlock(&hlock);
            
            wait_queue_size(&wait,&size);
            wait_queue_down(&wait);

            goto __again;
            //return -1;
        }
        
        *t = h2->value;
        head = h2;

        pthread_mutex_unlock(&hlock);
        
        __sync_fetch_and_sub(&size,1);
        if(h1 != &s)
            delete h1;
        
        return 0;
    }

    bool isEmpty(void)
    {
        return 0==__sync_fetch_and_add(&size,0);
    }
    
    ~Queue()
    {
        T t;
        while(0 == deQueue(&t))
            ;
        
        wait_queue_destroy(&wait);
    }
    
};

#endif
