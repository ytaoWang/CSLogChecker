/**
 * @file   Agent.h
 * @author wyt <wyt@wyt-laptop>
 * @date   Mon Apr 30 13:31:57 2012
 * 
 * @brief  Agent base class
 * @email:datao0907@163.com
 * 
 */

#ifndef __AGENT_H_
#define __AGENT_H_

#include "Error.h"
#include "BaseHeader.h"
#include "Manager.h"
//#include "Slab.h"

class BaseRequest;
class InReq;
class SocketAddress;

class Agent
{
 public:
    Agent():m_iRefCount(0),m_bIsRecycler(false),\
            m_iConnect(CONNECTING),m_iConnectTimes(-1)
    {}
    virtual ~Agent()
    {}
    virtual int recvData(void)
    {return SUCCESSFUL;}
    
    virtual int sendData(void)
    {return SUCCESSFUL;}
    
    virtual int recvReq(BaseRequest *req)
    {
        return SUCCESSFUL;
    }
    
    virtual void writeBack(bool result) {}
    virtual void readBack(InReq &req) { }
    virtual int doTask(void) {return SUCCESSFUL;}
    virtual int doTimer(unsigned int tv){return SUCCESSFUL;}
    
    
    virtual int connectAfter(bool yes)
    {
        return SUCCESSFUL;
    }
    
    virtual int connect(const SocketAddress &addr)
    {
        return SUCCESSFUL;
    }
    
    virtual int recyler(void)
    {
        m_bIsRecycler = true;
        return SUCCESSFUL;
    }
    
    bool isRecyler(void) const
    {
        return m_bIsRecycler;
    }
    
    virtual int getOppAddr(SocketAddress &addr)
    {
        return SUCCESSFUL;
    }
    int getErrno(void)
    {
        return errno;
    }
    Agent* ref(void)
    {
        ++m_iRefCount;
        return this;
    }
    
    long getRefCount(void) const
    {
        return m_iRefCount;
    }
    
    virtual void release(void)
    {
    }
    
    int getState(void) const
    {
        return m_iConnect;
    }
    
    virtual void setState(int st)
    {
        m_iConnect = st;
    }
    
    int allowReconnect(void) const
    {
        if(m_iConnectTimes < MAXRECONNECTTIMES)
            return true;
        else 
            return false;
    }
    
    int getConnectTimes(void) const
    {
        return m_iConnectTimes;
    }
    
    void resetConnect(void)
    {
        m_iConnectTimes = 0;
    }
protected:
    int m_iRefCount;
    bool m_bIsRecycler;
    int m_iConnect;
    int m_iConnectTimes;
};



#endif
