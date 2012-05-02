#include <errno.h>

#include "BaseTask.h"
#include "BufferManager.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Error.h"
#include "SocketAddress.h"
#include "Agent.h"
using namespace std;

BufferManager::BufferManager():m_pLastIov(NULL),
                               m_pAgent(NULL),
                               m_iOffset(0),
                               m_iLen(0),
                               m_bReadHead(false),
                               m_bInit(false)
{}

int BufferManager::writeDynamic(char *buf,size_t len,BaseTask *pTask)
{
    struct iov_req iov(buf,len,pTask);
    m_lIovList.push_back(iov);
    return SUCCESSFUL;
}

int BufferManager::writeDynamic(char *buf,size_t len,SocketAddress &addr,BaseTask *pTask)
{
    struct iov_req iov(buf,len,pTask);
    
    if(!addr.getAddr(iov.m_UDPAddr))
        return FAILED;
    
    m_lIovList.push_back(iov);
    
    return SUCCESSFUL;
}


int BufferManager::write(UDPSocket& sock)
{
    list<iov_req>::iterator aIt,pIt;
    int ret = -1;
    
    aIt = m_lIovList.begin();
    int download = 0 ;
    
    for(;aIt != m_lIovList.end();)
    {
        pIt = aIt;
        ret = sock.writeSocket((char *)(pIt->m_Iov.iov_base),pIt->m_Iov.iov_len,pIt->m_UDPAddr);
        if(ret < 0) 
        {
            if( EINTR == errno)
                continue;
            if(errno == EWOULDBLOCK)
                return SUCCESSFUL;
            else 
            {
                handleError("BufferManager::write(UDPScoket & sock)");
                if(pIt->m_pTask)
                    pIt->m_pTask->writeBack(false);
                else 
                {
                    if(m_pAgent) 
                        m_pAgent->writeBack(false);
                    else
                        handleError("BufferManager don't be relate with a Agent");
                }
                
                return FAILED;
            }
        }
        
        download += ret;
        if((size_t)ret < pIt->m_Iov.iov_len)
        {
            pIt->m_Iov.iov_base = (void *)(((char *)(pIt->m_Iov.iov_base)) + ret);
            pIt->m_Iov.iov_len -= (size_t)ret;
            pIt->m_bComplete = false;
            m_pLastIov = pIt->m_Iov.iov_base;
            break;
        }
        else 
        {
            if(pIt->m_bComplete)
            {
                if(pIt->m_Iov.iov_base)
                {
                    delete [](char *)(pIt->m_Iov.iov_base);
                    pIt->m_Iov.iov_base = NULL;
                }
            } else {
                if(m_pLastIov)
                {
                    delete [](char *)m_pLastIov;
                    m_pLastIov = NULL;
                }
            }
            
            ++aIt;
            m_lIovList.erase(pIt);
            if(pIt->m_pTask)
                pIt->m_pTask->writeBack(true);
            else 
            {
                if(m_pAgent)
                    m_pAgent->writeBack(true);
                else
                {
                    handleError("BufferManager don't be relate with a agent");
                    return FAILED;
                }
            }
            
        }
    }
    
    return download;
}


int BufferManager::write(TCPSocket &sock)
{
    const unsigned int reqlen = (unsigned int)m_lIovList.size();
    struct iovec outdata[reqlen];
    
    memset(outdata,0,reqlen * sizeof(struct iovec));
    
    list<iov_req>::iterator aIt,pIt;
    aIt = m_lIovList.begin();
    unsigned int num = 0;
    
    while( aIt != m_lIovList.end())
    {
        outdata[num].iov_base = aIt->m_Iov.iov_base;
        outdata[num].iov_len = aIt->m_Iov.iov_len;
        ++num;
        ++aIt;
    }
    
    int ret = -1;
    while((ret = sock.writevSocket(outdata,reqlen)) < 0)
    {
        if(errno == EINTR)
            continue;
        if( (errno != EWOULDBLOCK) || (errno != EAGAIN))
        {
            handleWriteError();
            return FAILED;
        }
    }
    
    int download = ret;
    aIt = m_lIovList.begin();
    while(ret > 0 && aIt != m_lIovList.end())
    {
        pIt = aIt++;
        if((unsigned int) ret >= pIt->m_Iov.iov_len)
        {
            ret -= pIt->m_Iov.iov_len;
            if(pIt->m_bComplete)
            {
                if(pIt->m_Iov.iov_base)
                {
                    delete [](char *)(pIt->m_Iov.iov_base);
                    pIt->m_Iov.iov_base = NULL;
                }
            } else {
                if(m_pLastIov)
                {
                    delete [](char *)m_pLastIov;
                    m_pLastIov = NULL;
                }
            } 
            
            if(pIt->m_pTask)
            {
                pIt->m_pTask->writeBack(true);
            } else 
            {
                if(m_pAgent)
                {
                    m_pAgent->writeBack(true);
                } else {
                    handleError("the BufferManager don't relate with a Agent");
                    return FAILED;
                }
            }
            m_lIovList.erase(pIt);
        } else 
        {
            if(pIt->m_bComplete)
            {
                m_pLastIov = pIt->m_Iov.iov_base;
                pIt->m_bComplete = false;
            }
            pIt->m_Iov.iov_base = (void *)(((char *)pIt->m_Iov.iov_base + ret));
            pIt->m_Iov.iov_len -= (unsigned int)ret;
            break;
        }
    }
    
    return download;
}

int BufferManager::read(TCPSocket &sock)
{
    int ret;
    
    //while(true) 
    {
        if(m_bInit)
        {
            m_InReq.ioBuf = NULL;
            memset(&m_InReq.m_msgHeader,0,HEADER_SIZE);
            m_iLen = HEADER_SIZE;
        }
        if(m_bReadHead)
        {
            if((ret = sock.readSocket(((char *)(&m_InReq.m_msgHeader)) + m_iOffset,HEADER_SIZE - m_iOffset)) < 0)
            {
                if(!((errno == EWOULDBLOCK)) || (errno == EINTR))
                {
                    handleSyscallError("BufferManager::read()");
                    return FAILED;
                }
                return SUCCESSFUL;
            }
            else if(ret == 0)
            {
                return FAILED;
            }
        
            m_iOffset += (unsigned int)ret;
            if(m_iOffset == HEADER_SIZE)
            {
                m_iOffset = 0;
                m_iLen = m_InReq.m_msgHeader.length;
                if(m_iLen > 0) 
                {
                    m_InReq.ioBuf = new char[m_iLen + 1];
                    memset(m_InReq.ioBuf,0,m_iLen + 1);
                    m_bReadHead = false;
                } else if(m_iLen == 0)
                {
                    m_InReq.ioBuf = NULL;
                    if(m_pAgent)
                    {
                        m_pAgent->readBack(m_InReq);
                        this->m_bInit = true;
                    }
                    this->m_bReadHead = true;
                    return SUCCESSFUL;
                } else {
                    m_bReadHead = true;
                    handleError("the data length is illegal.");
                    return FAILED;
                }
            
            }
        
        } else {
            if((ret = sock.readSocket(m_InReq.ioBuf + m_iOffset,m_iLen - m_iOffset)) < 0)
            {
                if(!((errno == EWOULDBLOCK) || (errno == EINTR))) 
                {
                    handleSyscallError("BufferManger::readSocket");
                    return FAILED;
                }
                
            } else if(ret == 0)
            {
                handleSyscallError("the peer close the connection");
                return FAILED;
            } 
            m_iOffset += (unsigned int)ret;
            if(m_iOffset == m_iLen)
            {
                m_InReq.ioBuf[m_iLen] = '\0';
                m_iOffset = 0;
                m_iLen = 0;
                m_bReadHead = true;
                if(m_pAgent)
                {
                    m_pAgent->readBack(m_InReq);
                    this->m_bInit = true;
                }
                if(m_InReq.ioBuf)
                {
                    delete []m_InReq.ioBuf;
                    m_InReq.ioBuf = NULL;
                }
                return SUCCESSFUL;
            }
        }
        
        return (int)(m_iLen - m_iOffset);
    }
}

int BufferManager::read(UDPSocket &sock)
{
    int ret ;
    
    m_InReq.ioBuf = NULL;
    memset(&m_InReq.m_msgHeader,0,HEADER_SIZE);
    m_iLen = HEADER_SIZE;
    
    if((ret = sock.readSocket(((char *)(&m_InReq.m_msgHeader)),HEADER_SIZE,m_InReq.oppoAddr)) < 0)
    {
        handleSyscallError("BufferManager::read UDPSocket");
        return FAILED;
    }
    
    else if((unsigned int) ret < HEADER_SIZE)
    {
        handleSyscallError("BufferManager::read(UDPSocket)");
        return FAILED;
    }

    m_iLen = m_InReq.m_msgHeader.length;
    if(m_iLen == 0)
    {
        m_InReq.ioBuf = NULL;
        if(m_pAgent)
            m_pAgent->readBack(m_InReq);

        return SUCCESSFUL;
    }
    
    m_InReq.ioBuf = new char[m_iLen + 1];
    memset(m_InReq.ioBuf,0,m_iLen + 1);
    if((ret =  sock.readSocket(m_InReq.ioBuf,m_iLen,m_InReq.oppoAddr)) < 0)
    {
        handleSyscallError("BufferManager::read UDPSocket data");
        return FAILED;
    }

    if((unsigned int)ret < m_iLen)
    {
        handleError("BufferManager::read UDPSocket data incomplete");
    }
    
    if(m_pAgent)
        m_pAgent->readBack(m_InReq);
    else
        handleError("BufferManager don't be relate with a Agent");

    delete [] (char *)m_InReq.ioBuf;
    m_bInit = true;
    
    return SUCCESSFUL;
}

void BufferManager::handleWriteError(void)
{
    list<iov_req>::iterator aIt = m_lIovList.begin();
    while(aIt != m_lIovList.end())
    {
        if(aIt->m_pTask)
            aIt->m_pTask->writeBack(false);
        else
            if(m_pAgent)
                m_pAgent->writeBack(false);
            else
                handleError("BufferManager don't be relate with a Agent");
        ++aIt;
    }
}

int BufferManager::clear(void)
{
    list<iov_req>::iterator aIt = m_lIovList.begin();
    while( aIt != m_lIovList.end())
    {
        if(aIt->m_bComplete && aIt->m_Iov.iov_base) 
        {
            delete [](char *)(aIt->m_Iov.iov_base);
            aIt->m_Iov.iov_base = NULL;
        }
        ++aIt;
    }
    
    m_lIovList.clear();
    if(m_pLastIov)
    {
        delete [](char *)m_pLastIov;
        m_pLastIov = NULL;
    }
    
    return 0;
}

BufferManager::~BufferManager()
{
    try 
    {
        m_pAgent = NULL;
        list<iov_req>::iterator aIt = m_lIovList.begin();
        while( aIt != m_lIovList.end())
        {
            if(aIt->m_bComplete  && aIt->m_Iov.iov_base)
            {
                delete [] (char*)aIt->m_Iov.iov_base;
                aIt->m_Iov.iov_base = NULL;
            }
            ++aIt;
        }
        
        m_lIovList.clear();
        if(m_pLastIov)
        {
            delete [] (char *)m_pLastIov;
            m_pLastIov = NULL;
        }
    } catch(...)
    {
        handleError("catch exception in ~BufferManager");
    }
}
