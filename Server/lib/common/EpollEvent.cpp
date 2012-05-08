#include <sys/epoll.h>

#include "EpollEvent.h"
#include "Epoll.h"
#include "Error.h"

const int TIMER_EVENT = 1<<0;
const int TIMER_TASK = 1<<1;
const int LEVELREVENT = EPOLLIN;
const int LEVELWEVENT = EPOLLOUT;
const int LEVELRWEVENT = LEVELREVENT | LEVELWEVENT | EPOLLERR | EPOLLHUP;
const int EVENTADD = EPOLL_CTL_ADD;
const int EVENTDEL = EPOLL_CTL_DEL;
const int EVENTCHANGE = EPOLL_CTL_MOD;


int EpollEvent::openREvent(void)
{
    if(-1 == m_iFd)
        return SUCCESSFUL;
    
    if(m_iEpollEvent & LEVELREVENT) return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTCHANGE,m_iEpollEvent | LEVELREVENT) < 0)
    {
        handleError("openREvent");
        return FAILED;
    }
    
    m_iEpollEvent |= LEVELREVENT;
    
    return SUCCESSFUL;
}

int EpollEvent::closeREvent(void)
{
    if(-1 == m_iFd) return SUCCESSFUL;
    
    if(!(m_iEpollEvent & LEVELREVENT)) 
        return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTCHANGE,m_iEpollEvent & (~LEVELREVENT)) < 0)
    {
        handleError("EpollEvent::CloseREvent.");
        return FAILED;
    }
    
    m_iEpollEvent &= ~LEVELREVENT;
    
    return SUCCESSFUL;
}


int  EpollEvent::openWEvent(void)
{
    if(-1 == m_iFd)
        return SUCCESSFUL;
    
    if(m_iEpollEvent & LEVELWEVENT) return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTCHANGE,m_iEpollEvent | LEVELWEVENT) < 0)
    {
        handleError("openWEvent");
        return FAILED;
    }
    
    m_iEpollEvent |= LEVELWEVENT;
    
    return SUCCESSFUL;
}


int  EpollEvent::closeWEvent(void)
{
    if(-1 == m_iFd) return SUCCESSFUL;
    
    if(!(m_iEpollEvent & LEVELWEVENT)) 
        return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTCHANGE,m_iEpollEvent & (~LEVELWEVENT)) < 0)
    {
        handleError("EpollEvent::CloseWEvent.");
        return FAILED;
    }
    
    m_iEpollEvent &= ~LEVELWEVENT;
    
    return SUCCESSFUL;
}


int EpollEvent::registerREvent(void)
{
    if(-1 == m_iFd) return FAILED;
    
    if(m_iEpollEvent & LEVELREVENT) return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTADD,m_iEpollEvent|LEVELREVENT) < 0)
    {
        handleError("EpollEvent::registerREvent");
        return FAILED;
    }
    
    m_iEpollEvent |= LEVELREVENT;

    return SUCCESSFUL;
}

int EpollEvent::unregisterREvent(void)
{
    if(-1 == m_iFd) return FAILED;
    
    if(!(m_iEpollEvent & LEVELREVENT)) return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTDEL,m_iEpollEvent & (~LEVELREVENT)) < 0)
    {
        handleError("EpollEvent::registerREvent");
        return FAILED;
    }
    
    m_iEpollEvent &= ~LEVELREVENT;

    return SUCCESSFUL;
}

int EpollEvent::registerWEvent(void)
{
    if(-1 == m_iFd) return FAILED;
    
    if(m_iEpollEvent & LEVELWEVENT) return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTADD,m_iEpollEvent | LEVELWEVENT) < 0)
    {
        handleError("EpollEvent::registerWEvent");
        return FAILED;
    }
    
    m_iEpollEvent |= LEVELWEVENT;
    
    return SUCCESSFUL;
}

int EpollEvent::unregisterWEvent(void)
{
    if(-1 == m_iFd) return FAILED;
    
    if(!(m_iEpollEvent & LEVELWEVENT)) return SUCCESSFUL;
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTDEL,m_iEpollEvent & (~LEVELWEVENT)) < 0)
    {
        handleError("EpollEvent::unregisterWEvent");
        return FAILED;
    }

    m_iEpollEvent &= ~LEVELWEVENT;

    return SUCCESSFUL;    
}

int EpollEvent::registerRWEvent(void)
{
    if( -1 == m_iFd)
        return SUCCESSFUL;
    
    if(m_iEpollEvent & LEVELREVENT)
        return registerWEvent();
    if(m_iEpollEvent & LEVELWEVENT)
        return registerREvent();
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTADD,LEVELRWEVENT) < 0)
    {
        handleError("EpollEvent::registerRWEvent");
        return FAILED;
    }
    
    m_iEpollEvent |= LEVELRWEVENT;
    
    return SUCCESSFUL;
}

int EpollEvent::unregisterRWEvent(void)
{
    if(-1 == m_iFd) 
        return SUCCESSFUL;
    
    if(!(m_iEpollEvent & LEVELREVENT))
        return unregisterWEvent();
    else if(!(m_iEpollEvent & LEVELWEVENT))
        return unregisterREvent();
    
    if(m_pEpoll->doEvent(this,m_iFd,EVENTDEL,LEVELRWEVENT) < 0)
    {
        handleError("EpollEvent::unregisterRWEvent");
        return FAILED;
    }
    
    m_iEpollEvent &= ~LEVELRWEVENT;
    
    return SUCCESSFUL;
}


int EpollEvent::registerTimer(unsigned int tv)
{
    if(!tv) return FAILED;
    if(m_iTTEvent & TIMER_EVENT) return SUCCESSFUL;
    
    if(m_pEpoll->attachTimer(this,tv)  < 0)
    {
        handleError("EpollEvent::registerTimer");
        return FAILED;
    }
    
    m_iTTEvent |= TIMER_EVENT;
    m_iTv = tv;
    #ifdef DEBUG
    std::cout << "TTEvent:" << m_iTTEvent << std::endl;
    #endif
    return SUCCESSFUL;
}

int EpollEvent::unregisterTimer(unsigned int tv)
{
    if(!tv) return SUCCESSFUL;
    
    #ifdef DEBUG
    std::cout << "TTEvent:" << m_iTTEvent << std::endl;
    #endif
    if(!(m_iTTEvent & TIMER_EVENT)) return SUCCESSFUL;
    
    if(m_pEpoll->delTimer(this,tv)  < 0)
    {
        handleError("EpollEvent::registerTimer");
        return FAILED;
    }
    
    m_iTTEvent &= ~TIMER_EVENT;
    
    return SUCCESSFUL;
}

int EpollEvent::registerTask(void)
{
    if(m_iTTEvent & TIMER_TASK) return SUCCESSFUL;
    
    if(m_pEpoll->attachTask(this)  < 0)
    {
        handleError("EpollEvent::registerTimer");
        return FAILED;
    }
    
    m_iTTEvent |= TIMER_TASK;
    
    return SUCCESSFUL;
}

int EpollEvent::unregisterTask(void)
{
    if(!(m_iTTEvent & TIMER_TASK)) return SUCCESSFUL;
    
    if(m_pEpoll->delTask(this)  < 0)
    {
        handleError("EpollEvent::registerTimer");
        return FAILED;
    }
    
    m_iTTEvent &= ~TIMER_TASK;
    
    return SUCCESSFUL;
}
