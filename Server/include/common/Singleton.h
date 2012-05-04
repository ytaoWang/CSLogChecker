#ifndef __SINGLETON_H_
#define __SINGLETON_H_

#include <iostream>
#include <typeinfo>

template <typename T>
class Singleton
{
public:
    static T* getInstance()
    {
        static T t;
        #ifdef DEBUG
        std::cout << typeid(T).name() << std::endl;
        #endif
        return &t;
    }
protected:
    Singleton()
    {
    }
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton &);
    ~Singleton()
    {
    }
};

    

#endif
