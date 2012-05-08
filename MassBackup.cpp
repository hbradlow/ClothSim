#ifndef _Mass_h
#define _Mass_h
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include "Object.cpp"
#define GRAVITY -5.81 

class Mass{
public:
    Eigen::Vector4f velocity;
    Eigen::Vector4f acceleration;
    Eigen::Vector4f position;
    Eigen::Vector4f temp;
    int pinned;
    std::vector<Mass*> links;
    std::vector<Object*> objects;
    void update(float dt);
    void commit();
    void f(Eigen::Vector4f *pos, Eigen::Vector4f *result);
    Mass();
};
Mass::Mass(){
    links = std::vector<Mass*>();
    objects = std::vector<Object*>();
    acceleration << 0,0,0,0;
    pinned = 0;
}
void Mass::f(Eigen::Vector4f *pos, Eigen::Vector4f *result){
    float stiff = 10.0f;
    Eigen::Vector4f a;
    a << 0,0,0,0;
    Eigen::Vector4f v;
    v << velocity.x(),velocity.y(),velocity.z(),0;
    for(int i = 0; i<links.size(); i++){
        Mass *link = links[i];
        float length = (*pos-link->position).norm();
        float stretch = length-2.0f/40;
        Eigen::Vector4f force;

        force = (stiff*(link->position-(*pos))*stretch);
        if(force.norm()>.2){
                force = force*(.2/force.norm());
        }
        a += force;
    }
    a.z() += .2f*GRAVITY*.0001f;
    for(int i = 0; i<objects.size(); i++){
            Eigen::Vector4f normal;
            if(objects[i]->intersects(&position,&normal))
            {
                v *= 0;
                a *= 0;
            }
    }
    v += a;
    *result = v;
}
void Mass::commit(){
        position = temp;
}
void Mass::update(float dt){
    if(pinned)
            return;
    Eigen::Vector4f guess;
    guess << position.x(),position.y(),position.z(),1;
    Eigen::Vector4f prev;
    prev << position.x(),position.y(),position.z(),1;
    Eigen::Vector4f vguess;
    vguess << velocity.x(),velocity.y(),velocity.z(),1;
    for(int i = 0; i<1; i++){
        f(&guess,&vguess);
        guess = guess+10*dt*vguess; 
    }
    temp = guess;
   // position = position + velocity * dt;
}
#endif
