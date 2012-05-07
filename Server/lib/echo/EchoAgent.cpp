#include "EchoAgent.h"
#include "Error.h"
#include "BaseHeader.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

void EchoAgent::readBack(InReq &req)
{
    cout << __FUNCTION__ <<endl;
    unpackage(req.m_msgHeader,req.ioBuf);
}

void EchoAgent::unpackage(struct MsgHeader &header,const char *msg)
{
    cout << "msg:" << msg << endl;
    
    this->package(header,msg);
}

void EchoAgent::package(struct MsgHeader &header,const char *msg)
{
    int len = strlen(msg);
    
    if(len != header.length) {
        header.length = len;
        handleError("header'length(%d) don't equal to msg(%d)\n",header.length,strlen(msg));
    }
    
    char *pBuf = new char[sizeof(struct MsgHeader) + len];
    memset(pBuf, 0,sizeof(MsgHeader) + len);
    
    #ifdef DEBUG
    cout << "header length:" << sizeof(header) << endl;
    #endif
    
    memcpy(pBuf,&header,sizeof(header));
    
    if(len) 
        memcpy(pBuf + sizeof(header),msg,len);
    
    if(this->writeDynData(pBuf,sizeof(header) + len,NULL) < 0) {
        cout << "------------>" << endl;
    }

    #ifdef DEBUG
    cout << "write msg:"<< msg << endl;
    #endif
}
