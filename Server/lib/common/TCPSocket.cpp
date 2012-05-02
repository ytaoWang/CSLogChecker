#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <error.h>

#include "Error.h"
#include "TCPSocket.h"
#include "SocketAddress.h"
#include "CSLC_const.h"
#include "BaseHeader.h"

int TCPSocket::m_iIpType = IPV4;

TCPSocket::TCPSocket():m_iSockFd(-1)
{
}

TCPSocket::TCPSocket(int fd):m_iSockFd(fd)
{
}

int TCPSocket::readSocket(char *buf,size_t len)
{
    return read(m_iSockFd,buf,len);
}

int TCPSocket::readnSocket(char *vptr,unsigned int n)
{
    unsigned int nleft;
    int nread;
    char *ptr;
    
    if(!vptr) return -1;

    ptr = vptr;
    nleft = n;
    
    while(nleft > 0)
    {
        if((nread = read(m_iSockFd,ptr,nleft)) < 0)
        {
            if(errno == EINTR)
                nread = 0;
            else if(!((errno == EWOULDBLOCK) || (errno == EAGAIN)))
            {
                handleSyscallError("readn");
                return -1;
            }
            
        } else if(nread == 0)
            break;
        else 
        {
            ptr += nread;
            nleft -= (unsigned int)nread;
        }
    }
    
    return (int)(n - nleft);
}

int TCPSocket::writeSocket(const char *buf,size_t len)
{    
    return write(m_iSockFd,buf,len);
}

int TCPSocket::writevSocket(const struct iovec *v,size_t len)
{
    return writev(m_iSockFd,v,len);
}

int TCPSocket::generateSocket(void)
{
    if(m_iIpType == IPV4 )
        m_iSockFd = socket(PF_INET,SOCK_STREAM,0);
    else
        m_iSockFd = socket(PF_INET6,SOCK_STREAM,0);
    
    if(m_iSockFd < 0) 
    {
        handleSyscallError("TCPSocket::generateSocket");
    }
    
    return m_iSockFd;
}

int TCPSocket::setNonblock(void)
{
    int val;
    if((val = fcntl(m_iSockFd,F_GETFL,0)) < 0)
    {
        handleSyscallError("TCPSocket::setNonblock");
        return FAILED;
    }
    
    val |= O_NONBLOCK;
    if(fcntl(m_iSockFd,F_SETFL,val) < 0)
    {
        handleSyscallError("TCPSocket::setNonblock");
        return FAILED;
    }
    
    return SUCCESSFUL;
}

int TCPSocket::enableReuseaddr(void)
{
    int val = 1;
    
    if(setsockopt(m_iSockFd,SOL_SOCKET,SO_REUSEADDR,(const void *)&val,sizeof(val)) < 0)
    {
        handleSyscallError("TCPSocket::enableReuseaddr");
        return FAILED;
    }
    
    return SUCCESSFUL;
}

int TCPSocket::disableLinger(void)
{
    struct linger ling = 
        {
            0,0
        };
    
    if(setsockopt(m_iSockFd,SOL_SOCKET,SO_LINGER,&ling,sizeof(ling)) < 0)
    {
        handleSyscallError("TCPSOCKET::disableLinger");
        return FAILED;
    }
    
    return SUCCESSFUL;
}

int TCPSocket::disableNagle(void)
{
    int val = 1;
    if(setsockopt(m_iSockFd,IPPROTO_TCP,TCP_NODELAY,(const void *)&val,sizeof(val)) < 0)
    {
        handleSyscallError("TCPSocket::disableNagle");
        return FAILED;
    }
    
    return SUCCESSFUL;
}


int TCPSocket::closeSocket(void)
{
    if(m_iSockFd == -1)
        return SUCCESSFUL;
    if(close(m_iSockFd) < 0)
    {
        handleSyscallError("TCPSocket::closeSocket");
        return FAILED;
    }
    
    m_iSockFd = -1;
    return SUCCESSFUL;
}


int TCPSocket::bindAddr(SocketAddress &serveraddr)
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
            handleSyscallError("TCPSocket::bindAddr");
            return FAILED;
        }

        if(serveraddr.getPort()==0)
        {
            handleError("TCPSocket::getPort");
            return FAILED;
        }
        addr.sin_port = htons(serveraddr.getPort());
        if(bind(m_iSockFd,(const struct sockaddr *)&addr,sizeof(addr)) <0)
        {
            handleSyscallError("TCPSocket::bindAddr");
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
                handleSyscallError("TCPSocket::bindAddr");
                return FAILED;
            }
            
        }
        
        if(serveraddr.getPort()==0)
        {
            handleError("TCPSocket::getPort");
            return FAILED;
        }
        addr.sin6_port = htons(serveraddr.getPort());
        if(bind(m_iSockFd,(const struct sockaddr *)&addr,sizeof(addr)) <0)
        {
            handleSyscallError("TCPSocket::bindAddr");
            return FAILED;
        }
    }
    return SUCCESSFUL;
}

int TCPSocket::listenOn(int qs)
{
    if(listen(m_iSockFd,qs)< 0)
    {
        handleSyscallError("TCPSocket::listenOn");
        return FAILED;
    }

    return SUCCESSFUL;
}
    
int TCPSocket::acceptConnection(SocketAddress &addr)
{
    if(IPV4 == m_iIpType)
    {
        struct sockaddr_in cliAddr;
        unsigned int cliAddrLen = sizeof(cliAddr);
        
        memset(&cliAddr,0,cliAddrLen);
        
        int fd = accept(m_iSockFd,(struct sockaddr *)&cliAddr,&cliAddrLen);
        
        addr.setAddress(inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port));
        return fd;
    } else 
    {
        struct sockaddr_in6 cliAddr;
        unsigned int cliAddrLen = sizeof(cliAddr);
        memset(&cliAddr,0,cliAddrLen);
        
        int fd = accept(m_iSockFd,(struct sockaddr *)&cliAddr,&cliAddrLen);
        char str[46];
        addr.setAddress(inet_ntop(AF_INET6,&cliAddr.sin6_addr,str,sizeof(str)),ntohs(cliAddr.sin6_port));
        return fd;
    }
    
    
    return FAILED;
}
    
int TCPSocket::setKeepAlive(void)
{
    int val = 1;
    if(setsockopt(m_iSockFd,SOL_SOCKET,SO_KEEPALIVE,(const void *) &val,sizeof(val)) < 0)
    {
        handleSyscallError("TCPSocket::setKeepAlive");
        return FAILED;
    }
        
    return SUCCESSFUL;
}

int TCPSocket::connectSocket(struct SocketAddress& ipaddr)
{
    int ret = -1;
    
    if(m_iIpType == IPV4)
    {
        struct sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        addr.sin_family = PF_INET;
        if(ipaddr.ifAnyAddr())
        {
            handleError("TCPSocket::connectSocket");
            return FAILED;
        }
        if(ipaddr.getPort() == 0)
        {
            handleError("TCPSocket::connectSocket getport");
            return FAILED;
        }
        if(!ipaddr.getAddr(addr))
        {
            handleError("TCPSocket::connectSocket getAddr error");
            return FAILED;
        }
        return connect(m_iSockFd,(const struct sockaddr*)&addr,sizeof(addr));
    }
    
    struct sockaddr_in6 addr;
    memset(&addr,0,sizeof(addr));
    addr.sin6_family = PF_INET6;
    if(ipaddr.ifAnyAddr())
    {
        handleError("TCPSocket::connect,ipaddr.ip AnyAddr");
        return FAILED;
    }
    else
    {
        if(inet_pton(PF_INET6,ipaddr.getIP(),&(addr.sin6_addr)) < 0)
        {
            handleSyscallError("TCPSocket::bindAddr::inet_address");
            return FAILED;
        }
        
        if(0 == ipaddr.getPort())
        {
            handleError("TCPSocket::connect port");
            return FAILED;
        }
        
        addr.sin6_port = htons(ipaddr.getPort());
        addr.sin6_scope_id = 2;
        return connect(m_iSockFd,(struct sockaddr*)&addr,sizeof(addr));
    }

    return FAILED;
}


int TCPSocket::peek(char *buf,size_t size)
{
    return recv(m_iSockFd,buf,size,MSG_PEEK);
}
    
int TCPSocket::throwData(unsigned int size)
{
    char arr[size];
    return read(m_iSockFd,arr,size);
}

