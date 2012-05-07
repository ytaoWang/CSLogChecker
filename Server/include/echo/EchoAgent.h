#ifndef __ECHO_AGENT_H_
#define __ECHO_AGENT_H_

#include <iostream>

#include "Slab.h"
#include "TCPAgent.h"
#include "Manager.h"

class Epoll;
class SocketAddress;
class TCPSocket;
struct MsgHeader;

class EchoAgent:public TCPAgent,public Slab<EchoAgent>
{
 public:
    EchoAgent(Epoll *epl):TCPAgent(epl)
    {
        std::cout << "EchoAgent (Epoll)"<<std::endl;
    }
    
    EchoAgent(const SocketAddress &addr,Epoll *epl):TCPAgent(addr,epl)
    {
        std::cout << "EchoAgent (Epoll,Socket)"<<std::endl;
    }
    
    EchoAgent(const TCPSocket &sock,const SocketAddress &addr,Epoll *epl)
        :TCPAgent(sock,addr,epl)
    {
        std::cout << "EchoAgent (all)"<<std::endl;
        if(m_eEpollEvent.registerTimer(2000) < 0) {
            handleError("EchoAgent::registerTimer");
        }
        
    }

    void readBack(InReq &req);
    void unpackage(struct MsgHeader &header,const char *msg);
    void package(struct MsgHeader &header,const char *msg);
    int doTimer(unsigned int tv);
    ~EchoAgent()
    {
        m_eEpollEvent.unregisterTimer(2000);
    }
    
    void release(void)
    {
        #ifdef DEBUG
        std::cout << "RefCount:"<< m_iRefCount << std::endl;
        #endif
        --m_iRefCount;
        if(m_iRefCount == 0) {
            Manager<EchoAgent>::getInstance()->recycler(this);
        }
    }
    
};



#endif
