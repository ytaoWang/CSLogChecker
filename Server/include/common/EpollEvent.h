#ifndef __EPOLL_EVENT_H_
#define __EPOLL_EVENT_H_

#include "TCPSocket.h"
#include "Agent.h"

#include <cstdlib>

class Epoll;
class EpollEvent
{
    unsigned int m_iEpollEvent;
    Agent * m_pHandler;
    Epoll * m_pEpoll;
    int  m_iFd;
    unsigned int m_iTTEvent;//timer task event
    unsigned int m_iTv;//time val
    bool m_bPersist;
 public:
 EpollEvent():m_iEpollEvent(0),m_iTTEvent(0),m_pHandler(NULL),\
        m_pEpoll(NULL),m_iFd(-1),m_iTv(0),m_bPersist(false)
    {

    }
    
    void setEpoll(Epoll *epl)
    {
        m_pEpoll = epl;
    }
    
    void setPersist(bool p)
    {
        m_bPersist = p;
    }
    
    bool isPersist(void) const
    {
        return m_bPersist;
    }
    
    Epoll* getEpoll(void)
    {
        return m_pEpoll;
    }
    
    void setHandler(Agent *pHandler)
    {
        m_pHandler = pHandler;
    }
    
    Agent* getHandler(void)
    {
        return m_pHandler;
    }
    
    void setFd(int fd)
    {
        m_iFd = fd;
        m_iTTEvent = 0;
        m_iEpollEvent = 0;
    }
    
    
    int openREvent(void);
    int closeREvent(void);
    int openWEvent(void);
    int closeWEvent(void);
    
    int registerREvent(void);
    int registerWEvent(void);
    int registerTimer(unsigned int);
    int registerTask(void);
    
    int registerRWEvent(void);
    int unregisterREvent(void);
    int unregisterWEvent(void);
    int unregisterRWEvent(void);
    int unregisterTimer(unsigned int);
    int unregisterTask(void);
    
};

#endif
