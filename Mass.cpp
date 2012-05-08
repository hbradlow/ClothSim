#ifndef _Mass_h
#define _Mass_h
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include "Object.cpp"
#include "Sphere.cpp"
#define GRAVITY -9.81 


class Mass{
public:
    struct Link {
        Mass *m;
        float k;
        float length;
    };
    Eigen::Vector4f velocity;
    Eigen::Vector4f acceleration;
    Eigen::Vector4f position;
    Eigen::Vector4f temp;
    int pinned;
    std::vector<Link> links;
    std::vector<Object*> objects;
    void update(float dt);
    void updateSphere(float dt);
    void commit();
    void f(Eigen::Vector4f *pos, Eigen::Vector4f *result,float dt);
    Mass();
};
Mass::Mass(){
    pinned = 0;
    links = std::vector<Link>();
    objects = std::vector<Object*>();
    acceleration << 0,0,0,0;
}
void Mass::f(Eigen::Vector4f *pos, Eigen::Vector4f *result, float dt){
    float stiff = 20.0f;
    Eigen::Vector4f a;
    a << 0,0,0,0;
    Eigen::Vector4f v;
    v << velocity.x(),velocity.y(),velocity.z(),0;
    for(int i = 0; i<links.size(); i++){
        Mass *link = links[i].m;
        float length = (*pos-link->position).norm();
        float stretch = length-links[i].length;
        Eigen::Vector4f force;
        force = (links[i].k*(link->position-(*pos))*stretch);
        a += force;
    }
    a.z() += .001f*GRAVITY;
/*    for(int i = 0; i<objects.size(); i++){
            Eigen::Vector4f normal;
            if(objects[i]->intersects(&position,&normal))
            { 
                float factor = normal.norm()/((Sphere*)objects[i])->radius;
                factor *= factor*factor;
                factor *= factor*factor;
                a += .05f*normal.normalized()/(factor*factor*factor*factor);
                normal = normal/normal.norm();
                v += -1.0f*normal*(normal.dot(velocity))/(factor*factor*factor*factor);
           //     v *= 0;
           //     a *= 0;
            }
    }*/
    v += a*dt;
    *result = v;
}
void Mass::commit(){
    if(pinned==0){
        position = temp;
    }
}
void Mass::updateSphere(float dt){
    if(pinned==0){
        for(int i = 0; i<objects.size(); i++){
            Eigen::Vector4f normal;
            if(objects[i]->intersects(&position,&normal))
            { 
                position = position + normal.normalized()*(((Sphere*)objects[i])->radius - normal.norm());
                velocity *= 0;
            }
        }
    }
}
void Mass::update(float dt){
    if(pinned==0){
        Eigen::Vector4f guess;
        guess << position.x(),position.y(),position.z(),1;
        Eigen::Vector4f vguess;
        for(int i = 0; i<50; i++){
            f(&guess,&vguess,dt);
            guess = position+dt*vguess; 
        }
        velocity = vguess;
        position = guess;
        temp = guess;
    }
}


#endif
