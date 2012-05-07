#ifndef __ECHO_TASK_H_
#define __ECHO_TASK_H_

#include "TCPAgent.h"

class Epoll;

class EchoTask :public TCPAgent
{
 public:
    EchoTask(Epoll *epl);
    int doTask(void);
    ~EchoTask(void);
};

#endif
