#ifndef __EPOLL_H_
#define __EPOLL_H_

#include <list>
#include <vector>

class EpollEvent;

class Epoll {
public:
    Epoll(void);
    virtual ~Epoll(void);
    int getEpollFd(void) const
    {
        return m_iEventFd;
    }
    
    int initialize(unsigned int);
    int doEvent(EpollEvent* ptr,int fd,int op,unsigned int events);
    int attachTask(EpollEvent *ptr);
    int attachTimer(EpollEvent *ptr,unsigned int time);
    int delTask(EpollEvent *ptr);
    int delTimer(EpollEvent *ptr,unsigned int time);
    void run(void);
    
    
private:
    
    int getLastTime(void);
    void doTimer(void);
    void doTask(void);
    
    struct Timer_t
    {
        EpollEvent *ptr;
        unsigned int time;
        Timer_t::Timer_t(void),ptr(NULL),time(0)
        {}
        bool operator==(const Timer_t&t) const 
        {
            return ptr == t.ptr && time == t.time;
        }
        bool operator<(const Timer_t &t) const 
        {
            return absolute < t.absolute;
        }
        
        unsigned long absolute;
    };

    class CompareTimer 
    {
    public:
        //for min_heap
        bool operator()(const struct Timer_t *&t1 const struct Timer_t *&t2) const 
        {
            return !(*t1 < *t2);
        }
    };
    

    struct epoll_event* m_pEpollEvent;
    int m_iEpollFd;
    unsigned int  m_iEventSize;
    std::vector<struct Timer_t*> m_vAgentTimer;
    std::list<EpollEvent *> m_lAgentList;
};



#endif
