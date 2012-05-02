#ifndef __BASETASK_H_
#define __BASETASK_H_

#include "Error.h"

class BaseReq;
class BaseRequest;
class InReq;

class BaseTask
{
 public:
    BaseTask() {}
    virtual ~BaseTask() {}
    virtual int recvReq(BaseRequest *)
    {
        return SUCCESSFUL;
    }
    
    virtual void readBack(InReq& req) {}
    virtual void writeBack(bool result) {}
};



#endif
