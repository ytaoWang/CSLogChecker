#ifndef __RUN_CONTROL_H_
#define __RUN_CONTROL_H_

#include "Singleton.h"
#include "Epoll.h"
#include "TCPListenAgent.h"
#include "EchoAgent.h"

class RunControl : public Singleton<RunControl>
{
protected:
    RunControl()
    {
        m_pEpoll = Singleton<Epoll>::getInstance();
        m_pEchoListenAgent = new TCPListenAgent<EchoAgent>(m_pEpoll);
    }
    
    ~RunControl()
    {
        delete m_pEpoll;
        delete m_pEchoListenAgent;
    }
	friend class Singleton<RunControl>;
    
 public:
    void init();
    void run();
private:
    Epoll* m_pEpoll;
    TCPListenAgent<EchoAgent> *m_pEchoListenAgent;
};



#endif
