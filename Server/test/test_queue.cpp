#include <iostream>

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include <queue>
#include <cstdlib>

using namespace std;

queue<unsigned long> q;

static int global1,global2;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

const int NUM = 100000;


void* execute1(void *value)
{
    (void)value;
    int i;
    
    unsigned long v;
    
    i = 0;
    
    pthread_mutex_lock(&lock);
    
    while(!q.empty()) {
        i ++ ;
        __sync_fetch_and_add(&global1,1);
        q.pop();
    }
    
    pthread_mutex_unlock(&lock);
    
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
        __sync_fetch_and_add(&global2,1);
        
        pthread_mutex_unlock(&lock);
        
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
    
    for(int i = 0;i < tnum;++i) 
    {
        if(pthread_create(&tid[i],NULL,execute1,(void *)tnum) < 0)
        {
            cout << "create consumer thread error." << endl;
        }
    }

    for(int i = 0; i < tnum;++i)
        pthread_join(tid[i],NULL);
    
    cout << "consumer:" << global1 << ",producer:"<< global2 << endl;
    return 0;
}
