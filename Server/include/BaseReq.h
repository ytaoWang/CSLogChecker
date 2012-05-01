#ifndef __BASEREQ_H_
#define __BASEREQ_H_

#include <netinet/in.h>

#include "BaseHeader.h"

struct BaseRequest
{
    BaseRequest() {}
    virtual ~BaseRequest() 
    {}
};

struct InReq
{
    MsgHeader m_msgHeader;
    char *ioBuf;
    sockaddr_in oppoAddr;
};

    
    
#endif
