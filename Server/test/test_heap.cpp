#include <vector>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

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
    struct Timer_t t[4];

    for(int i = 0;i< 4;++i)
    {    
        t[i].time = random();
        v.push_back(&t[i]);
    }
    
    for(vector<struct Timer_t *>::iterator it = v.begin(); it != v.end(); ++it)
    {
        cout <<"it:"<<(*it)->time<<endl;
    }

    make_heap(v.begin(),v.end(),CompareTimer());
    
    for(vector<struct Timer_t *>::iterator it = v.begin(); it != v.end(); ++it)
    {
        cout <<"it:"<<(*it)->time<<endl;
    }
    return 0;
}
