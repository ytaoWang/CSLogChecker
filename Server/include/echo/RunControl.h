#ifndef __RUN_CONTROL_H_
#define __RUN_CONTROL_H_

#include "EchoAgent.h"
#include "Singleton.h"
#include "Epoll.h"
#include "TCPListenAgent.h"
#include "EchoTask.h"

class RunControl : public Singleton<RunControl>
{
protected:
    RunControl()
    {
        m_pEpoll = Singleton<Epoll>::getInstance();
        m_EchoListenAgent.setEpoll(m_pEpoll);
        m_pEchoTask = new EchoTask(m_pEpoll);
    }
    
    ~RunControl()
    {
        delete m_pEchoTask;
    }
	friend class Singleton<RunControl>;
    
 public:
    void init();
    void run();
private:
    Epoll* m_pEpoll;
    TCPListenAgent<EchoAgent> m_EchoListenAgent;
    EchoTask* m_pEchoTask;
};



#endif
