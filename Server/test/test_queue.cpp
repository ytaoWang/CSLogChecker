#include <iostream>

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include <queue>
#include <cstdlib>
#include <sys/time.h>

#include "wait_queue.h"

using namespace std;

queue<unsigned long> q;

static int global1,global2;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

const int NUM = 100000;
wait_queue_t wait;
int tnum2 = 0;

void* execute1(void *value)
{
    (void)value;
    int i;
    bool empty;
    
    unsigned long v;
    
    while(1) 
    {

        pthread_mutex_lock(&lock);
        
        empty = q.empty();
        
        //pthread_mutex_unlock(&lock);
        
        if(!empty)
        {
            wait_queue_down(&wait);
            __sync_add_and_fetch(&global1,1);

            //  pthread_mutex_lock(&lock);
            q.pop();
            //pthread_mutex_unlock(&lock);
            i++;
        }

        pthread_mutex_unlock(&lock);
        
        if(__sync_add_and_fetch(&global1,0) == tnum2 * NUM)
            break;
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
    
    for(i = 0; i < NUM;++i) 
    {
        v = random();
        t ++ ;

        pthread_mutex_lock(&lock);
        q.push(v);
        pthread_mutex_unlock(&lock);

        __sync_fetch_and_add(&global2,1);
        wait_queue_up_nonblock(&wait);
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
    struct timeval tv1,tv2;
    

    if(argc != 3) {
        cout << "usage:" << argv[0] << " consumer producer" << endl;
        return -1;
    }
    
    tnum = atoi(argv[1]);
    tnum2 = atoi(argv[2]);

    pthread_t tid[tnum + tnum2];

    wait_queue_init(&wait);
    gettimeofday(&tv1,NULL);

    for(int i = 0;i < tnum2;++i) 
    {
        if(pthread_create(&tid[i+tnum],NULL,execute2,(void *)tnum2) < 0)
        {
            cout << "create producer thread error." << endl;
        }
    }
    
    for(int i = 0;i < tnum;++i) 
    {
        if(pthread_create(&tid[i],NULL,execute1,(void *)tnum) < 0)
        {
            cout << "create consumer thread error." << endl;
        }
    }

    for(int i = 0; i < tnum;++i)
        pthread_join(tid[i],NULL);
    for(int i = 0 ;i < tnum2 ; ++i) {
        pthread_join(tid[i+tnum],NULL);
    }
    
    wait_queue_destroy(&wait);

    gettimeofday(&tv2,NULL);
    
    cout << "consumer:" << global1 << ",producer:"<< global2 << endl;
    cout << "sec:" << (tv2.tv_sec - tv1.tv_sec) << ",tv1,usec:" << tv1.tv_usec << ",tv2 usec:" << tv2.tv_usec << endl;
    double  d;
    
    d = ((tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec))/1000000.00;
    
    cout << "sec:" << d << endl;
    
    return 0;
}
