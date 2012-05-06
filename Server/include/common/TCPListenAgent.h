#ifndef __TCP_LISTEN_AGENT_H_
#define __TCP_LISTEN_AGENT_H_

#include "Error.h"
#include "Agent.h"
#include "TCPSocket.h"
#include "EpollEvent.h"
#include "Epoll.h"
#include "SocketAddress.h"
#include "BaseHeader.h"
#include "Manager.h"

#include <netinet/tcp.h>
#include <iostream>
#include <cstring>
#include <errno.h>

#define LISTEN_NUM 50

using namespace std;

template<class ConcreteAgent>
class TCPListenAgent:public Agent
{
	TCPSocket m_tcpListenSocket;
	SocketAddress m_socketAddress;
	EpollEvent m_eEpollEvent;
	Epoll *m_pEpoll;

public:
 	TCPListenAgent():m_pEpoll(NULL)
    {
        m_eEpollEvent.setHandler(this);
    }

 	TCPListenAgent(Epoll *ep):m_pEpoll(ep)
	{
        m_eEpollEvent.setHandler(this);
        m_eEpollEvent.setEpoll(ep);
	}
	
    void setEpoll(Epoll *epl)
    {
        m_pEpoll = epl;
        m_eEpollEvent.setEpoll(epl);
    }
    
	~TCPListenAgent() {}
	
	virtual int init(SocketAddress &);
	virtual int recvData(void);
	virtual int sendData(void);
	
};

template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::init(SocketAddress &addr)
{
	m_socketAddress = addr;
	
	if(m_tcpListenSocket.generateSocket() < 0
		|| m_tcpListenSocket.disableLinger() <0
		|| m_tcpListenSocket.disableNagle() <0
		|| m_tcpListenSocket.enableReuseaddr() < 0
		|| m_tcpListenSocket.setNonblock() <0
		|| m_tcpListenSocket.bindAddr(m_socketAddress) <0
		|| m_tcpListenSocket.listenOn(LISTEN_NUM) <0)
	{
		handleSyscallError("TCPListenAgent::init()");
		return FAILED;
	}
	
	m_eEpollEvent.setFd(m_tcpListenSocket.getSockFd());
	m_eEpollEvent.setHandler(this);
	m_eEpollEvent.registerREvent();
	return SUCCESSFUL;
}

template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::recvData(void)
{
	SocketAddress peerAddress;
	
	int connfd;
	
	connfd = m_tcpListenSocket.acceptConnection(peerAddress);
	if(connfd < 0)
	{
		if(!((errno == EINTR) || (errno == EWOULDBLOCK)))
		{
			handleError("TCPListenAgent listen");
			return FAILED;
		}
	}

    cout << "peerAddr" << peerAddress.convertToString() << endl;

	TCPSocket connSock(connfd);
	if(connSock.setNonblock() <0
		|| connSock.disableLinger() < 0
		|| connSock.disableNagle() < 0)
	{
		handleError("TCPListenAgent::recvData");
		connSock.closeSocket();
		return FAILED;
	}

	ConcreteAgent *agent = \
        Manager<ConcreteAgent>::getInstance()->creat(connSock,peerAddress,m_pEpoll);
	return SUCCESSFUL;
}

template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::sendData(void)
{
	return FAILED;
}

#endif
