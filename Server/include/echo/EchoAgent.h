#ifndef __ECHO_AGENT_H_
#define __ECHO_AGENT_H_

#include "TCPAgent.h"

class Epoll;
class SocketAddress;
class TCPSocket;
struct MsgHeader;

class EchoAgent:public TCPAgent
{
    EchoAgent(Epoll *epl):TCPAgent(epl)
    {
    }
    
    EchoAgent(const SocketAddress &addr,Epoll *epl):TCPAgent(addr,epl)
    {
    }
    
    EchoAgent(const TCPSocket &sock,const SocketAddress &addr,Epoll *epl)
        :TCPAgent(sock,addr,epl)
    {
    }

    void readBack(struct InReq &req);
    void unpackage(struct MsgHeader &header,const char *msg);
    void package(struct MsgHeader &header,const char *msg);
};



#endif
