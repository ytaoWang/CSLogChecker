/**
 * @file   SocketAddress.h
 * @author wyt <wyt@wyt-laptop>
 * @date   Mon Apr 30 14:30:44 2012
 * 
 * @brief  
 * @email:datao0907@163.com
 * 
 */

#ifndef __SOCKETADDRESS_H_
#define __SOCKETADDRESS_H_

#include <string>
#include <netinet/in.h>



class SocketAddress
{
 public:
    SocketAddress(unsigned short port);
    SocketAddress();
    SocketAddres(const char *buf,unsigned short port);
    
    const char* getIP(void) const
    {
        return m_strIp.c_str();
    }
    
    unsigned short getPort(void) const
    {
        return m_iPort;
    }
    void setAddress(const char *buf,unsigned short p);
    sockaddr_in getAddr(void);
    bool getAddr(sockaddr_in &addr);
    void setPort(unsigned short port);
    const string &convertToString(void);
    bool ifAnyAddr(void) const;
    bool operator==(const SocketAddress&) const;
    ScoketAddress& operator = (const SocketAddress&);
    SocketAddress(const SocketAddress &addr);
    
 private:
    std::string m_strIp;
    std::string m_strStr;
    unsigned short m_iPort;
    static int m_iIpType;
};

#endif
