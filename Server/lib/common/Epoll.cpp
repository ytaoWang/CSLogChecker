/**
 * @file   Epoll.cpp
 * @author wyt <wyt@wyt-laptop>
 * @date   Sat Apr 28 18:55:03 2012
 * 
 * @brief  
 * @email:datao0907@163.com
 * 
 */

#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include "Epoll.h"
#include "Error.h"

Epoll::Epoll(void):m_iEpollFd(-1),m_iEventSize(0),m_pEpollEvent(NULL)
{}

Epoll::~Epoll(void)
{
    if(m_pEpollEvent != NULL)
        delete [] m_pEpollEvent;
}

int Epoll::initialize(unsigned int size)
{
    m_iEventSize = size;
    
    if((m_iEpollFd = epoll_create(size)) < 0) {
        handleSyscallError("epoll_create");
        return FAILED;
    }
    
    m_pEpollEvent = new epoll_event[size];
    memset(m_pEpollEvent,0,sizeof(struct epoll_event)*size);
    return SUCCESSFUL;
}

int Epoll::doEvent(EpollEvent *ptr,int fd,int op,unsigned int events)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(struct epoll_event));
    ev.events = events;
    ev.data.ptr = ptr;
    
    if(epoll_ctl(m_iEpollFd,op,fd,&ev) < 0) 
    {
        handleSyscallError("doEvent");
        return FAILED;
    }
    
    return SUCCESSFUL;
}


int Epoll::attachTimer(EpollEvent *ptr,unsigned int time)
{
    struct timer_t *handler = new struct timer_t;
    struct timeval tv;

    if(gettimeofday(&tv,NULL) < 0) {
        handleSyscallError("attachTimer");
        return FAILED;
    }

    handler->ptr = ptr;
    handler->time = time;
    handler->absolute = tv.tv_sec * 1000 + tv.tv_usec/1000 + time;

    m_vAgentTimer.push_back(handler);

    return SUCCESSFUL;
}

int Epoll::delTimer(EpollEvent *ptr,unsigned int time)
{
    struct timer_t t;

    t.ptr = ptr;
    t.time = time;
    
    for(vector<struct timer_t *>::iterator it = m_vAgentTimer.begin();\
        it != m_vAgentTimer.end();++it)
    {
        if(**it == t) {
            m_vAgentTimer.erase(it);
            break;
        }
    }
    
    return SUCCESSFUL;
}

int Epoll::attachTask(EpollEvent *ptr)
{
    if(!ptr) {
        handleError("attachTask");
        return FAILED;
    }
    
    m_lAgentList.push_back(ptr);
    return SUCCESSFUL;
}

int Epoll::delTask(EpollEvent *ptr)
{
    for(list<EpollEvent*>::it = m_lAgentList.begin();it != m_lAgentList.end();++it)
    {
        if(*it == ptr) 
        {
            m_lAgentList.erase(it);
            break;
        }
        
    }
    
    return SUCCESSFUL;
}

int Epoll::getLastTime(void)
{    
    struct timeval tv;
    unsigned long now;

    if(gettimeofday(&tv,NULL) < 0) {
        handleSyscallError("attachTimer");
        return FAILED;
    }
 
    make_heap(m_vAgentTimer.begin(),m_vAgentTimer.end(),CompareTimer());
    
    now = tv.tv_sec * 1000 + tv.tv_usec/1000;
    
    return m_vAgentTimer[0]->absolute - now;
}

void Epoll::doTask(void)
{
    for(std::list<EpollEvent *>::iterator it = m_lAgengList.begin();it != m_lAgentList.end();++it)
    {
        //   (*it)->doTask();
    }
}


void Epoll::doTimer(void)
{
    unsigned long now;

    if(gettimeofday(&tv,NULL) < 0) {
        handleSyscallError("attachTimer");
        return FAILED;
    }

    now = tv.tv_sec * 1000 + tv.tv_usec/1000;

    for(vector<struct Timer_t *>::iterator it = m_vAgentTimer.begin(); it != m_vAgentTimer.end();++it)
    {
        if((*it)->absolute <= now || ((*it)->absolute - now) < TIMER_ACCURACY) {
            (*it)->ptr->doTimer((*it)->time);
            if((*it)->ptr->isPersistTimer()) {
                //update absolute
                (*it)->absolute = now + (*it)->time;
            } else {
                m_vAgentTimer.erase(it);
            }   
        }
        break;
    }
    
}

void Epoll::run(void)
{
    
}



