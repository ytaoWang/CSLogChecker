#ifndef __UDPSOCKET_H_
#define __UDPSOCKET_H_

class UDPSocket
{
    int m_iSockFd;
    static int m_iIpType;
 public:
    UDPSocket();
    int bindAddr(const SocketAddress& addr);
    int readSocket(char *buf,size_t len,struct sockaddr_in &cliaddr);
    int writeSocket(const char *buf,size_t len,const struct sockaddr_in & cliaddr);
    int generateSocket(void);
    int setNonblock(void);
    int enableReuseaddr(void);
    int enableBroadcast(void);
    int getSockFd(void) const
    {
        return m_iSockFd;
    }
    int closeSocket(void);
};



#endif
