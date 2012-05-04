#include <iostream>

#include "Manager.h"
#include "Error.h"

template <class T>
T* Manager<T>::creat(Epoll *epl)
{
    
}

template <typename T>
T* Manager<T>::creat(const SocketAddress & addr,Epoll *epl)
{
    
}

template <typename T>
T* Manager<T>::creat(const TCPSocket& sock,const SocketAddress &addr,Epoll *epl)
{
    
}

template <typename T>
void Manager<T>::recycler(T * pT)
{
    
}

template <typename T>
void Manager<T>::recycler(void)
{
    
}
