#include "RunControl.h"

void RunControl::init(void)
{
    m_pEpoll->initialize(100);
    SocketAddress addr("192.168.1.137",8888);
    m_pEchoListenAgent->init(addr);
}

void RunControl::run(void)
{
    m_pEpoll->run();
}
