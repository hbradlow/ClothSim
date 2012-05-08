#ifndef _Object_h
#define _Object_h
#include <iostream>
#include <vector>
#include <Eigen/Core>

class Object{
public:
    Eigen::Vector4f position;
    virtual int intersects(Eigen::Vector4f* pos, Eigen::Vector4f* normal){
        return 0;
    }
};
#endif
