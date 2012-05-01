#ifndef __TCP_SOCKET_H_
#define __TCP_SOCKET_H_

#include <sys/uio.h>
class SocketAddress;

class TCPSocket
{
    int m_iSockFd;
    static int m_iIpType;
 public:
    TCPSocket();
    TCPSocket(int fd);
    int readSocket(char* buf,size_t len) const;
    int readnSocket(char *vptr,unsigned int n) const;
    int writeSocket(const char *buf,size_t len)const;
    int writevSocket(const struct iovec *v,size_t len) const;
    int generateSocket(void);
    int setNonblock(void);
    int enableReuseaddr(void);
    int disableLinger(void);
    int disableNagle(void);
    int getSockFd(void) const
    {
        return m_iSockFd;
    }
    
    int bindAddr(SocketAddress &);
    int listenOn(int qs);
    int acceptConnection(SocketAddress &);
    int closeSocket(void);
    int setKeepAlive(void);
    int connectSocket(const struct SocketAddress& addr);
    //for debug
    int peek(char* buf,size_t size);
    void throwData(unsigned int size);
};



#endif
