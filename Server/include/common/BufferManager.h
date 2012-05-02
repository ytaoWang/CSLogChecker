#ifndef __BUFFER_MANAGER_H_
#define __BUFFER_MANAGER_H_

#include <sys/uio.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <list>
#include <cstring>

#include "BaseReq.h"
#include "BaseHeader.h"
#include "Error.h"

class TCPSocket;
class UDPSocket;
class BaseTask;
class SocketAddress;
class Agent;

class BufferManager 
{
    struct iov_req 
    {
    iov_req():m_bComplete(true),m_pTask(NULL)
        {}
    iov_req(char *buf,size_t len,BaseTask *pTask = NULL)
    : m_bComplete(true),m_pTask(pTask)
        {
            m_Iov.iov_base = buf;
            m_Iov.iov_len = len;
            bzero(&m_UDPAddr,sizeof(struct sockaddr_in));
        }
        
        struct iovec m_Iov;
        bool m_bComplete;
        struct sockaddr_in m_UDPAddr;
        BaseTask *m_pTask;
    };
    
 public:
    BufferManager(void);
    ~BufferManager(void);
    int writeDynamic(char *,size_t len,BaseTask * = NULL);
    int writeDynamic(char *,size_t len,SocketAddress &,\
                     BaseTask * = NULL);
    int write(TCPSocket &);
    int write(UDPSocket &);
    
    int read(TCPSocket &);
    int read(UDPSocket &);
    
    void setAgent(Agent *agent)
    {
        m_pAgent = agent;
    }
    void handleWriteError(void);
    int clear(void);
    size_t getLength(void) const
    {
        return m_lIovList.size();
    }
    
 private:
    void *m_pLastIov;
    Agent *m_pAgent;
    off_t m_iOffset;
    size_t m_iLen;
    bool m_bReadHead;
    bool m_bInit;
    std::list<iov_req> m_lIovList;
    struct InReq m_InReq;
};


#endif
