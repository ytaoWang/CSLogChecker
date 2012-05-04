#ifndef __BASE_HEADER_H_
#define __BASE_HEADER_H_

#include <sys/types.h>
#include <stdint.h>

const int CONNECTING = 0;
const int CONNECTED = 1;
const int CONNECTFAILED = -1;
const int MAXRECONNECTTIMES = 3;

const int IPV4 = 4;
const int IPV6 = 6;

const char* const IPV4ANYADDR = "0.0.0";
const char* const IPV6ANYADDR = "::1";
//#define IPV4ANYADDR "0.0.0"
//#define IPV6ANYADDR "::1"

struct MsgHeader 
{
    uint32_t cmd;
    uint32_t length;
    uint32_t error;
    uint32_t para1;
    uint32_t para2;
    
MsgHeader():
    cmd(0),length(0),
        error(0),para1(0),para2(0)
    {
    }
    
    ~MsgHeader()
    {
    }
    
};

    
const unsigned int HEADER_SIZE = sizeof(struct MsgHeader);

#endif
