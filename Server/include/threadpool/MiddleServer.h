#ifndef __MIDDLE_SERVER_H_
#define __MIDDLE_SERVER_H_

/*
 * Middler Server Base class
 * task dispatcher
 * Agent <---> Middle Server <----> Thread
 */

#include "Agent.h"
class Task;

class MiddleServer:public Agent {
 protected:
    Agent * m_pAgent;
    Task * m_pTask;
    
 public:
 	MiddleServer(Agent *pAgent,Task *pTask=NULL):m_pAgent(pAgent),m_pTask(pTask)
    {}
 	MiddleServer():m_pAgent(NULL),mpTask(NULL)
    {}
    
    void setHandler(Task *pTask) {
        m_pTask = pTask;
    }

    Task *getHandler(void){
        return m_pTask;
    }
    
    void setAgent(Agent *pAgent) {
        m_pAgent = pAgent;
    }
    
    Agent* getAgent(void) {
        return m_pAgent;
    }
    
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
