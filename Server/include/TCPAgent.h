#ifndef __TCPAGENT_H_
#define __TCPAGENT_H_

#include "Agent.h"
#include "BaseTask.h"
#include "BaseHeader.h"
#include "SocketAddress.h"
#include "EpollEvent.h"
#include "BufferManager.h"
#include "CSLC_const.h"

class Epoll;
class TCPSocket;

const SocketAddress InitAddr("0",0);
class TCPAgent:public Agent 
{
 public:
    TCPAgent():m_Addr(InitAddr) {}
    TCPAgent(Epoll *epl);
    TCPAgent(const SocketAddress&,Epoll *);
    TCPAgent(const TCPSocket&,const SocketAddress&,Epoll *);
    virtual ~TCPAgent();
    int connect(const SocketAddress &);
    virtual int init();
    virtual int recyler(void);
    virtual int sendData(void);
    virtual int recvData(void);
    virtual int doParse(void)
    {
        return SUCCESSFUL;
    }
    
    virtual int doPackage(void)
    {
        return SUCCESSFUL;
    }
    
    virtual void readBack(InReq& req)
    {
    }
    
    virtual void writeBack(bool result)
    {
    }
    
    virtual int connectAfter(bool);
    int writeDynData(char *buf,size_t len,BaseTask *= NULL);
    int writeData(void);
    
    int readData(void);
    virtual int getOppAddr(SocketAddress& sockAddr)
    {
        if(InitAddr == m_Addr)
            return FAILED;
        sockAddr = m_Addr;
        return SUCCESSFUL;
    }
    
    const EpollEvent& getEpollEvent() const
    {
        return m_eEpollEvent;
    }
    
    void setState(int st);
    
protected:
    TCPSocket m_tTcpSocket;
    SocketAddress m_Addr;
    MsgHeader m_msgHeader;//may be useless
    EpollEvent m_eEpollEvent;
    int m_iLen;
    BufferManager m_bBufv;
    bool m_bActive;
};

#endif
