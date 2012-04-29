#ifndef __EPOLL_EVENT_H_
#define __EPOLL_EVENT_H_

#include "TCPSocket.h"
#include "Agent.h"

class Agent;
class EpollEvent
{
    unsigned int m_iEpollEvent;
    Agent * m_pHandler;
    Epoll * m_pEpoll;
    int  m_iFd;
    unsigned int m_iTTEvent;
    unsigned int m_iTv;
 public:
 EpollEvent():m_iEpollEvent(0),m_iTTEvent(0),m_pHandler(NULL),m_pEpoll(NULL),m_iFd(-1),tv(0)
    {

    }
    
    void setEpoll(Epoll *epl)
    {
        m_pEpoll = epl;
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
    
    int registerREvents(void);
    int registerWEvents(void);
    int registerTimer(unsigned int);
    int registerTask(void);
    
    int registerRWEvents(void);
    int unregisterRWEvents(void);
    int unregisterTimer(unsigned int);
    int unregisterTask(void);
    
};

#endif
