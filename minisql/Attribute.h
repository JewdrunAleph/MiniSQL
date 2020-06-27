#ifndef Attribute_h
#define Attribute_h

#include <string>
#include <iostream>
using namespace std;

class Attribute
{
public:
    string name;
    int type;                  //attribute的类型,-1代表浮点数, 0代表整型, 其他的正数代表char，value为char的类型
    bool ifUnique;
    string index;              // 默认值为空，即没有index
    Attribute(string n, int t, bool i);
    
public:
    int static const TYPE_FLOAT = -1;
    int static const TYPE_INT = 0;
    string indexNameGet(){return index;}
    
    void print()
    {
        cout <<  "name: " << name << ";type: " << type << ";ifUnique: " << ifUnique << ";index: " << index << endl;
    }
};

#endif
