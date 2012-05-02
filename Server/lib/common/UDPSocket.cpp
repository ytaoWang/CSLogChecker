#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstring>
#include <cstdio>

#include "SocketAddress.h"
#include "BaseHeader.h"
#include "Error.h"
#include "CSLC_const.h"
#include "UDPSocket.h"

int UDPSocket::m_iIpType = IPV4;

UDPSocket::UDPSocket():m_iSockFd(-1)
{}

int UDPSocket::bindAddr(const SocketAddress& serveraddr)
{
    if( m_iIpType == IPV4)
    {
        struct sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        addr.sin_family = PF_INET;
        
        if(serveraddr.ifAnyAddr())
            addr.sin_addr.s_addr = htons(INADDR_ANY);
        else if((inet_aton(serveraddr.getIP(),(in_addr *)&addr.sin_addr.s_addr)) == 0)
        {
            handleSyscallError("UDPSocket::bindAddr");
            return FAILED;
        }

        if(serveraddr.getPort()==0)
        {
            handleError("UDPSocket::getPort");
            return FAILED;
        }
        addr.sin_port = htons(serveraddr.getPort());
        if(bind(m_iSockFd,(const struct sockaddr *)&addr,sizeof(addr)) <0)
        {
            handleSyscallError("UDPSocket::bindAddr");
            return FAILED;
        }
    } else 
    {
        struct sockaddr_in6 addr;
        memset(&addr,0,sizeof(addr));
        addr.sin6_family = PF_INET6;
        addr.sin6_scope_id = 2;
        if( serveraddr.ifAnyAddr())
            addr.sin6_addr = in6addr_any;
        else
        {
            if( inet_pton(PF_INET6,serveraddr.getIP(),&(addr.sin6_addr)) < 0)
            {
                handleSyscallError("UDPSocket::bindAddr");
                return FAILED;
            }
            
        }
        
        if(serveraddr.getPort()==0)
        {
            handleError("UDPSocket::getPort");
            return FAILED;
        }
        addr.sin6_port = htons(serveraddr.getPort());
        if(bind(m_iSockFd,(const struct sockaddr *)&addr,sizeof(addr)) <0)
        {
            handleSyscallError("UDPSocket::bindAddr");
            return FAILED;
        }
    }
    return SUCCESSFUL;
}

int UDPSocket::readSocket(char *buf,size_t len,struct sockaddr_in &addr)
{
    socklen_t socklen = sizeof(addr);
    
    return recvfrom(m_iSockFd,buf,len,0,
                    (struct sockaddr*)&addr,&socklen);
}

int UDPSocket::writeSocket(const char *buf,size_t len,const struct sockaddr_in &addr)
{
    return sendto(m_iSockFd,buf,len,0,(const struct sockaddr *)&addr,
                  sizeof(addr));
}


int UDPSocket::generateSocket(void)
{
    if(m_iIpType == IPV4 )
        m_iSockFd = socket(PF_INET,SOCK_STREAM,0);
    else
        m_iSockFd = socket(PF_INET6,SOCK_STREAM,0);
    
    if(m_iSockFd < 0) 
    {
        handleSyscallError("UDPSocket::generateSocket");
    }
    
    return m_iSockFd;
}

int UDPSocket::setNonblock(void)
{
int val;
    if((val = fcntl(m_iSockFd,F_GETFL,0)) < 0)
    {
        handleSyscallError("UDPSocket::setNonblock");
        return FAILED;
    }
    
    val |= O_NONBLOCK;
    if(fcntl(m_iSockFd,F_SETFL,val) < 0)
    {
        handleSyscallError("UDPSocket::setNonblock");
        return FAILED;
    }
    
    return SUCCESSFUL;
}

int UDPSocket::enableReuseaddr(void)
{
    int val = 1;
    
    if(setsockopt(m_iSockFd,SOL_SOCKET,SO_REUSEADDR,(const void *)&val,sizeof(val)) < 0)
    {
        handleSyscallError("UDPSocket::enableReuseaddr");
        return FAILED;
    }
    
    return SUCCESSFUL;
}

int UDPSocket::enableBroadcast(void)
{
    int val = 1;
    
    if(setsockopt(m_iSockFd,SOL_SOCKET,SO_BROADCAST,(const void *)&val,sizeof(val)) < 0)
    {
        handleSyscallError("UDPSocket::enableBroadcast");
        return FAILED;
    }
    
    return SUCCESSFUL;
}

int UDPSocket::closeSocket(void)
{
    if(m_iSockFd == -1)
        return SUCCESSFUL;
    if(close(m_iSockFd) < 0)
    {
        handleSyscallError("UDPSocket::closeSocket");
        return FAILED;
    }
    
    m_iSockFd = -1;
    return SUCCESSFUL;
}
