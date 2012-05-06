#include "RunControl.h"

void RunControl::init(void)
{
    m_pEpoll->initialize(100);
    SocketAddress addr("127.0.0.1",8888);
    m_EchoListenAgent.init(addr);
}

void RunControl::run(void)
{
    m_pEpoll->run();
}
