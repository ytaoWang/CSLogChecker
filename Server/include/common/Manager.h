#ifndef __MANAGER_H_
#define __MANAGER_H_

#include <list>

#include "Singleton.h"
#include "Error.h"

class TCPSocket;
class SocketAddress;
class Epoll;
/*
template <
    class T,
    template <class U> class S=Singleton 
>
*/
template <typename T>
class Manager:public Singleton<Manager<T> >
{
public:
 	Manager():m_iFree(0),m_iNew(0)
    {}
    
    ~Manager()
    {
        if(m_iFree != m_iNew)
        {
            handleError("object %s number isn't equal,delete:%d,new:%d\n",\
                        typeid(T).name(),m_iFree,m_iNew);
        }
    }
    
	friend class Singleton<Manager>;
public:
    T* creat(Epoll *epl)
    {
        T * pT = new T(epl);
        m_iNew++;

        return pT;
    }

    T* creat(const SocketAddress &addr,Epoll *epl)
    {
        T *pT = new T(addr,epl);
        m_iNew++;
        
        return pT;
    }

    T* creat(const TCPSocket &sock,const SocketAddress &addr,Epoll *epl)
    {
        T *pT = new T(sock,addr,epl);
        m_iNew ++;
    
        return pT;
    }

    void recycler(void)
    {
        if(m_listT.empty()) 
            return;

        for(typename std::list<T*>::iterator it = m_listT.begin();  \
            it != m_listT.end();)
        {
            m_listT.erase(it++);
            m_iFree--;
            delete it;
        }
    }
    

    void recycler(T *pT)
    {
        if(pT->getRefCount()) 
            return;
    
        m_listT.push_back(pT);
    }
    
 private:
    std::list<T*> m_listT;
    unsigned int m_iFree,m_iNew;
};    

#endif
