#ifndef _Sphere_h
#define _Sphere_h
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include "Object.cpp"

class Sphere: public Object{
public:
    Eigen::Vector4f position;
    float radius;
    float v;
    float a;
    Sphere(){
        a = -0.000005f;
        v = -.001f;
    }
int intersects(Eigen::Vector4f* pos, Eigen::Vector4f* normal){
        if((*pos-position).norm()<=radius){
            *normal = (*pos-position); 
            return 1;
        }
        else
            return 0;
}
void update(){
        v += a;
        position.z() += v;
        if(position.z()<-1.15f)
        {
                v = -v;
        }
}
};
#endif
