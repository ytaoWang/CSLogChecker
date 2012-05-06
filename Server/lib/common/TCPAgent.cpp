#include <iostream>
#include <cstring>
#include <fstream>
#include <errno.h>

#include "TCPAgent.h"
#include "Error.h"
#include "TCPSocket.h"

using namespace std;

TCPAgent::TCPAgent(Epoll *epl):m_bActive(true),\
                               m_iLen(0)
{
    m_eEpollEvent.setEpoll(epl);
    this->m_bBufv.setAgent(this);
}

TCPAgent::TCPAgent(const SocketAddress& oppoAddr,Epoll *epl):m_Addr(oppoAddr),m_iLen(0),m_bActive(true)
{
    m_eEpollEvent.setEpoll(epl);
    this->m_bBufv.setAgent(this);
}

TCPAgent::TCPAgent(const TCPSocket& sock,\
                   const SocketAddress& oppoAddr,Epoll *epl):
    m_tTcpSocket(sock),m_Addr(oppoAddr),m_iLen(0),m_bActive(false)
{
    m_iConnectTimes = -1;
    setState(CONNECTED);
    m_eEpollEvent.setEpoll(epl);
    m_bBufv.setAgent(this);
    m_eEpollEvent.setFd(m_tTcpSocket.getSockFd());
    m_eEpollEvent.setHandler(this);
    
    if(m_eEpollEvent.registerREvent() < 0)
    {
        handleError("TCPAgent::TCPAgent");
    }
}

int TCPAgent::init()
{
    return SUCCESSFUL;
}

TCPAgent::~TCPAgent()
{
    try
    {
        if(this->recycler() < 0)
        {
            handleError("TCPAgent::~TCPAgent");
        }
        
    } catch(...)
    {
        handleError("TCPAgent::TCPAgent");
    }
}

int TCPAgent::recycler(void)
{
    if(m_bIsRecycler)
    {
        return SUCCESSFUL;
    }
    
    m_bIsRecycler = true;
    
#ifdef DEBUG
    if(false == m_bActive)
    {
        string ip;
        ip.assign(m_Addr.getIP());
        string msg = "*** Client Closed,IP is "+ip+"****";
        handleError(msg.c_str());
    }
#endif
    if(m_eEpollEvent.unregisterRWEvent()< 0)
    {
        handleError("TCPAgent::recycler");
    }
    
    m_eEpollEvent.setFd(-1);
    m_eEpollEvent.setHandler(NULL);
    if(this->m_tTcpSocket.closeSocket() < 0)
        handleError("TCPAgent::recycler");
    return SUCCESSFUL;
}

int TCPAgent::connect(const SocketAddress &addr)
{
    m_iConnectTimes += 1;
    if(m_tTcpSocket.closeSocket() < 0)
    {
        handleError("TCPAgent::connect");
    }
    
    m_Addr = addr;
    if(m_tTcpSocket.generateSocket() < 0
       || m_tTcpSocket.disableNagle() < 0
       || m_tTcpSocket.disableLinger() <0
       || m_tTcpSocket.setNonblock() < 0
       || m_tTcpSocket.setKeepAlive() < 0)
    {
        handleError("TCPAgent::connect");
        return FAILED;
    }
    
    m_eEpollEvent.setFd(m_tTcpSocket.getSockFd());
    m_eEpollEvent.setHandler(this);
    if(m_eEpollEvent.registerRWEvent() < 0)
        return FAILED;
    if(m_tTcpSocket.connectSocket(m_Addr) < 0)
        return FAILED;
    return SUCCESSFUL;
}

int TCPAgent::sendData(void)
{
    return writeData();
}

int TCPAgent::recvData(void)
{
    return readData();
}

int TCPAgent::writeData(void)
{
    if(this->m_bBufv.write(m_tTcpSocket) < 0)
    {
        handleError("TCPAgent::writeData");
        return FAILED;
    }
    
    if(this->m_bBufv.getLength() == 0)
    {
        if(this->m_eEpollEvent.closeWEvent() < 0)
        {
            handleError("TCPAgent::writeData");
            return FAILED;
        }
    }
    
    return SUCCESSFUL;
}

int TCPAgent::writeDynData(char *buf,size_t len,BaseTask *pTask)
{
    if(this->m_bBufv.writeDynamic(buf,len,pTask) != SUCCESSFUL)
    {
        handleError("TCPAgent::writeDyncData");
        return FAILED;
    }
    
    if(this->m_eEpollEvent.openWEvent() < 0)
    {
        handleError("TCPAgent::writeDyncData");
        return FAILED;
    }

    return SUCCESSFUL;
}

int TCPAgent::readData()
{
    return this->m_bBufv.read(m_tTcpSocket);
}

int TCPAgent::connectAfter(bool result)
{
    return SUCCESSFUL;
}

void TCPAgent::setState(int st)
{
    m_iConnect = CONNECTED;
    if(st == CONNECTED)
    {
        if(m_eEpollEvent.closeWEvent() < 0)
        {
            handleError("TCPAgent::setState");
        }
        
    }
}

int TCPAgent::doTask(void)
{
    return SUCCESSFUL;
}

int TCPAgent::doTimer(unsigned int tv)
{
    return SUCCESSFUL;
}
