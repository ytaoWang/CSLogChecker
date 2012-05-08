#include "EchoTask.h"
#include "Manager.h"
#include "EchoAgent.h"
#include "Error.h"

EchoTask::EchoTask(Epoll *epl):TCPAgent(epl)
{
    if(m_eEpollEvent.registerTask() < 0) {
        handleError("EchoTask::registerTask");
    }
    
}

int EchoTask::doTask(void)
{
    Manager<EchoAgent>::getInstance()->recycler();
}

EchoTask::~EchoTask(void)
{
}
