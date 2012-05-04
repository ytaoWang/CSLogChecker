#include <iostream>

#include "RunControl.h"

using namespace std;

int main(void)
{    
    //cout <<"Hello,World"<<endl;
    RunControl::getInstance()->run();
    return 0;
}
