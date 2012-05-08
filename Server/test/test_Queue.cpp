#include <iostream>

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include "Queue.h"

using namespace std;

Queue<unsigned long> queue;

static int global1,global2;
static sem_t finish;

const int NUM = 100000;

void* execute1(void *value)
{
    (void)value;
    int i;
    
    unsigned long v;
    
    i = 0;
    
    //sem_wait(&finish);
    
    while(queue.deQueue(&v) == 0) {
        i ++ ;
        __sync_fetch_and_add(&global1,1);
    }
    
    #ifdef DEBUG
    cout << "consumer pid:" << pthread_self() <<",dequeue num:" << i << endl;
    #endif

    return NULL;    
}


void* execute2(void *value)
{
    unsigned long v;
    int num ,i,t;
    
    num = (int)value;
    t = 0;
    
    for(i = 0; i < NUM ;++i) 
    {
        v = random();
        t ++ ;
        queue.enQueue(v);
        __sync_fetch_and_add(&global2,1);
    }

    #ifdef DEBUG
    cout << "producer pid:" << pthread_self() <<",enqueue num:" << t << " total:" << global2 << " one:" << num << endl;
    #endif
    
    return NULL;
}


int main(int argc,char *argv[])
{
    int tnum;
    unsigned long v = 1;

    if(argc != 3) {
        cout << "usage:" << argv[0] << " consumer producer" << endl;
        return -1;
    }
   
    sem_init(&finish,0,0);
    
    tnum = atoi(argv[1]);
    int tnum2 = atoi(argv[2]);

    pthread_t tid[tnum + tnum2];

    for(int i = 0;i < tnum2;++i) 
    {
        if(pthread_create(&tid[i+tnum],NULL,execute2,(void *)tnum2) < 0)
        {
            cout << "create producer thread error." << endl;
        }
    }
    
    for(int i = 0 ;i < tnum2 ; ++i) {
        pthread_join(tid[i+tnum],NULL);
    }
    
    //sem_post(&finish);
    
    for(int i = 0;i < tnum;++i) 
    {
        if(pthread_create(&tid[i],NULL,execute1,(void *)tnum) < 0)
        {
            cout << "create consumer thread error." << endl;
        }
    }

    for(int i = 0; i < tnum;++i)
        pthread_join(tid[i],NULL);

    sem_destroy(&finish);
    
    cout << "consumer:" << global1 << ",producer:"<< global2 << endl;
    return 0;
}
