#ifndef __MIDDLE_SERVER_H_
#define __MIDDLE_SERVER_H_

/*
 * Middler Server Base class
 * task dispatcher
 * Agent <---> Middle Server <----> Thread
 */

#include "Agent.h"

class MiddleServer:public Agent {
 public:
    virtual int putTaskFromAgent(unsigned long)
    {
        return SUCCESSFUL;
    }
    
    virtual int getTaskFromAgent(unsigned long)
    {
        return SUCCESSFUL;
    }
    
    virtual int putResultFromThread(unsigned long)
    {
        return SUCCESSFUL;
    }
    
    virtual int recvData(void)
    {
        return SUCCESSFUL;
    }
    
    virtual int sendData(void)
    {
        return SUCCESSFUL;
    }
    
    
    int recycler(void) 
    {
        return SUCCESSFUL;
    }
    
};

#endif
