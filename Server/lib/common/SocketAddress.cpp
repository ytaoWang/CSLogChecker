#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>

#include <string>

#include "SocketAddress.h"
#include "Error.h"
#include "BaseHeader.h"

using namespace std;

const int IPV4LEN = 15;
const int IPV6LEN = 39;
const int MAXSOCKADDRLEN = 100;
const char *ANYADDR = "0";

//extern  const char* IPV4ANYADDR;
//extern const char* IPV6ANYADDR;

int SocketAddress::m_iIpType = IPV4;

SocketAddress::SocketAddress():m_iPort(0),m_strStr("")
{
    if(m_iIpType == IPV4)
        m_strIp = IPV4ANYADDR;
    else
        m_strIp = IPV6ANYADDR;
}

SocketAddress::SocketAddress(unsigned short port)
{
    SocketAddress();
    m_iPort = port;
}

SocketAddress::SocketAddress(const char *buf,unsigned short port)
{
    m_iPort = port;
    if(m_iIpType == IPV4) 
        if(strlen(buf) > IPV4LEN || strcmp(buf,ANYADDR) == 0)
            m_strIp = IPV4ANYADDR;
        else
            m_strIp = string(buf);
    else 
    {
        if(strlen(buf) > IPV6LEN || strcmp(buf,ANYADDR) == 0)
            m_strIp = IPV6ANYADDR;
        else
            m_strIp = string(buf);
    }
    
}

void SocketAddress::setPort(unsigned short port)
{
    m_iPort = port;
}

void SocketAddress::setAddress(const char *buf,unsigned short port)
{
    m_iPort = port;
    
    if(m_iIpType == IPV4 )
    {
        if(strlen(buf) <= IPV4LEN && strcmp(buf,ANYADDR) != 0)
            m_strIp = string(buf);
    } else {
        if(strlen(buf) <= IPV6LEN && strcmp(buf,ANYADDR) != 0)
            m_strIp = string(buf);
    }
}

const string& SocketAddress::convertToString(void)
{
    char buf[ MAXSOCKADDRLEN ] = { 0 };
    
    sprintf(buf,"%s %u",m_strIp.c_str(),m_iPort);
    m_strStr = buf;

    return m_strStr;
}

bool SocketAddress::ifAnyAddr(void) const
{
    if( m_iIpType == IPV4)
        if(IPV4ANYADDR == m_strIp)
            return true;
        else return false;
    else if(IPV6ANYADDR == m_strIp)
        return true;
    else return false;
}

bool SocketAddress::operator == (const SocketAddress &h) const
{
    if(m_strIp == h.m_strIp && m_iPort == h.m_iPort)
        return true;
    else 
        return false;
}

SocketAddress& SocketAddress::operator= (const SocketAddress &h)
{
    if(this == &h)
        return *this;
    
    m_strIp = h.m_strIp;
    m_iPort = h.m_iPort;
    m_strStr = h.m_strStr;
    
    return *this;
}

sockaddr_in SocketAddress::getAddr(void)
{
    struct sockaddr_in addr;
    
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_strIp.c_str());
    addr.sin_port = htons(m_iPort);
    
    return addr;
}

bool SocketAddress::getAddr(sockaddr_in &addr)
{
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_iPort);
    
    return inet_pton(AF_INET,m_strIp.c_str(),&addr.sin_addr) == 1;
}

SocketAddress::SocketAddress(const SocketAddress &addr) {
    this->m_strIp = addr.m_strIp;
    this->m_iPort = addr.m_iPort;
    this->m_strStr = addr.m_strStr;
}
