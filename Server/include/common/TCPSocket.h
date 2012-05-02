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
    int readSocket(char* buf,size_t len);
    int readnSocket(char *vptr,unsigned int n);
    int writeSocket(const char *buf,size_t len);
    int writevSocket(const struct iovec *v,size_t len);
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
    int connectSocket(struct SocketAddress& addr);
    //for debug
    int peek(char* buf,size_t size);
    int throwData(unsigned int size);
};



#endif
