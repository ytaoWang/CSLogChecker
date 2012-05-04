#ifndef __MANAGER_H_
#define __MANAGER_H_

template <typename T>
class Manager::public Singleton<Manager>
{
protected:
    Manager();
    ~Manager();
	friend class Singleton<Manager>;
public:
    
};    

#endif
