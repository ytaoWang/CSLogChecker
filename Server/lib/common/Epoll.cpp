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

#include <algorithm>

#include "Epoll.h"
#include "EpollEvent.h"
#include "Error.h"
#include "CSLC_const.h"
#include "SocketAddress.h"

using namespace std;

Epoll::Epoll(void):m_iEpollFd(-1),m_iEventSize(0),m_pEpollEvent(NULL)
{}

Epoll::~Epoll(void)
{
    if(m_pEpollEvent != NULL)
        delete [] m_pEpollEvent;
    for(vector<struct Timer_t *>::iterator aIt = m_vAgentTimer.begin(); aIt != m_vAgentTimer.end();)
        delete (*aIt);
    m_lAgentList.clear();
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
    struct Timer_t *handler = new struct Timer_t;
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
    struct Timer_t t;

    t.ptr = ptr;
    t.time = time;
    
    for(vector<struct Timer_t *>::iterator it = m_vAgentTimer.begin();\
        it != m_vAgentTimer.end();++it)
    {
        if(**it == t) {
            m_vAgentTimer.erase(it);
            delete (*it);
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
    for(std::list<EpollEvent*>::iterator it = m_lAgentList.begin();it != m_lAgentList.end();++it)
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
        return TIMER_ACCURACY;
    }
 
    make_heap(m_vAgentTimer.begin(),m_vAgentTimer.end(),Epoll::CompareTimer());
    
    now = tv.tv_sec * 1000 + tv.tv_usec/1000;
    
    return m_vAgentTimer[0]->absolute - now;
}

void Epoll::doTask(void)
{
    Agent *pAgent;
    
    for(std::list<EpollEvent *>::iterator it = m_lAgentList.begin();it != m_lAgentList.end();++it)
    {
        pAgent = (*it)->getHandler();
        pAgent->doTask();
    }
}


void Epoll::doTimer(void)
{
    unsigned long now;
    Agent *pAgent;
    struct timeval tv;
    
    if(gettimeofday(&tv,NULL) < 0) {
        handleSyscallError("attachTimer");
        return;
    }

    now = tv.tv_sec * 1000 + tv.tv_usec/1000;

    for(vector<struct Timer_t *>::iterator it = m_vAgentTimer.begin(); it != m_vAgentTimer.end();++it)
    {
        if((*it)->absolute <= now || ((*it)->absolute - now) < TIMER_ACCURACY) {
            pAgent = (*it)->ptr->getHandler();
            pAgent->doTimer((*it)->time);
            if((*it)->ptr->isPersist()) {
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
    int nfds = 0;
    EpollEvent *pEpollEvent = NULL;
    int timeout;
    
    while(true)
    {
        timeout = getLastTime();
        if(timeout < 0)
        {
            this->doTimer();
            continue;
        }
        
        if((nfds = epoll_wait(m_iEpollFd,m_pEpollEvent,m_iEventSize,timeout)) < 0)
        {
            if(errno == EINTR) continue;
            else 
                handleSyscallError("Epoll::epoll_wait");
        }
        
        for(int i = 0;i < nfds;i++)
        {
            pEpollEvent = static_cast<EpollEvent *>(m_pEpollEvent[i].data.ptr);
            if(!pEpollEvent)
                goto _task;
            Agent *agent = pEpollEvent->getHandler();
            if(!agent)
                goto _task;
            if(m_pEpollEvent[i].events & EPOLLERR || m_pEpollEvent[i].events & EPOLLHUP)
            {
                if(agent->getState() == CONNECTING)
                {
                    if(agent->getErrno() == EISCONN)
                    {
                        agent->setState(CONNECTED);
                        if(agent->connectAfter(true) < 0)
                        {
                            agent->release();
                            continue;
                        }
                        agent->resetConnect();
                        continue;
                    } else 
                    {
                        if(agent->allowReconnect())
                        {
                            SocketAddress addr;
                            if(SUCCESSFUL == (agent->getOppAddr(addr)))
                            {
                                if(agent->connect(addr) < 0)
                                {
                                    agent->release();
                                }
                            }
                            else 
                            {
                                handleError("get peer address error");
                                agent->release();
                            }

                            continue;
                        }
                        else
                        {
                            agent->resetConnect();
                            if(agent->connectAfter(false) < 0)
                            {
                                handleError("connect error");
                                agent->release();
                                continue;
                            }
                            else continue;
                        }
                    }
                } else 
                {
                    if(agent->recvData() < 0)
                    {
                        handleError("agent recvData");
                        agent->release();
                        continue;
                    }
                    
                }
                continue;
            }

            if(m_pEpollEvent[i].events & EPOLLIN)
            {
                if(agent->recvData() < 0)
                {
                    agent->release();
                    continue;
                }
                
            }

            if(m_pEpollEvent[i].events & EPOLLOUT)
            {
                if(CONNECTED == agent->getState())
                {
                    if(agent->sendData() < 0)
                    {
                        agent->release();
                        continue;
                    }
                    
                } else 
                {
                    agent->setState(CONNECTED);
                    if(agent->connectAfter(true) < 0)
                    {
                        agent->release();
                        continue;
                    }
                    
                }
            }
        }
    _task:
        this->doTask();
    }
    
}



