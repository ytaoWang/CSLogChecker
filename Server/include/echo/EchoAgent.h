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
    }

    void readBack(InReq &req);
    void unpackage(struct MsgHeader &header,const char *msg);
    void package(struct MsgHeader &header,const char *msg);
    void release(void)
    {
        --m_iRefCount;
        if(!m_iRefCount) {
            Manager<EchoAgent>::getInstance()->recycler(this);
        }
        
    }
    
};



#endif
