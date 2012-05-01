#include <vector>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <list>
using namespace std;

struct Timer_t 
{
    int time;
    bool operator<(const Timer_t &t) const{
        return time < t.time;
    }
    
};
class CompareTimer 
{
public:
    bool operator()(struct Timer_t *&a,struct Timer_t *&b) const
    {
        return !(*a < *b);
    } 
};

    
int main(void)
{
    vector<struct Timer_t *> v;
    list<struct Timer_t *> l;
    struct Timer_t t[4];
    struct Timer_t *p;
    
    for(int i = 0;i< 4;++i)
    {    
        p = new Timer_t;
        p->time = random();
        l.push_front(p);
        //v.push_back(p);
    }
    
    for(vector<struct Timer_t *>::iterator it = v.begin(); it != v.end(); ++it)
    {
        cout <<"it:"<<(*it)->time<<endl;
    }

    make_heap(v.begin(),v.end(),CompareTimer());
    cout <<"after make heap:"<<endl;
    
    for(vector<struct Timer_t *>::iterator it = v.begin(); it != v.end(); ++it)
    {
        cout <<"it:"<<(*it)->time<<endl;
    }

    for(list<struct Timer_t *>::iterator it = l.begin(),aIt = l.begin();it!=l.end();)//it = aIt,aIt++)
    {
        cout <<"delete It:"<< (*it)->time <<endl;
        delete (*it);
        l.erase(it++);
    }

    cout <<"after delete"<<endl;
    for(list<struct Timer_t *>::iterator it = l.begin(); it != l.end(); ++it)
    {
        cout <<"it:"<<(*it)->time<<endl;
    }

    return 0;
}
