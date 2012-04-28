#ifndef __EPOLL_H_
#define __EPOLL_H_

class EpollEvent;

class Epoll {
public:
    Epoll(void);
    virtual ~Epoll(void);
    int getEpollFd(void) const;
    int doEvent(EpollEvent* ptr,int fd,int op,unsigned int events);
    int attachTimer(EpollEvent *ptr,unsigned int timer);
    int delTimer(EpollEvent *ptr,unsigned int timer);
    void run(void);
    
    
private:
    struct epoll_event* m_pEpollEvent;
    int m_iEpollFd;
    unsigned int m_iEventSize;
    std::vector<EpollEvent *> m_vAgentTimer;
    std::list<EpollEvent *> m_lAgentList;
};



#endif
