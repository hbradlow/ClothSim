#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include "Mass.cpp"
#include "Sphere.cpp"
#define HEIGHT 800
#define WIDTH 800
using namespace std;
vector<Mass*> masses = vector<Mass*>();
float r = .02f;
int width = 50;
int updateCount = 0;
int imageCount = 0;
void init(void) 
{
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);

   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_position1[] = { -1.0, -1.0, -5.0, 1.0 };

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
}
void screendump(int index, int W, int H) {
        string name1 = "images/screenshot";
        string name2 = ".tga";
        string name3;
        std::stringstream namestr;
        namestr << index;
        name3 = namestr.str();
        string name = name1 + name3 + name2;

        FILE   *out = fopen((char*)name.c_str(),"wb");
        char   *pixel_data = new char[3*W*H];
        short  TGAhead[] = { 0, 2, 0, 0, 0, 0, W, H, 24 };
//        unsigned char  TGAhead[] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};

        glReadBuffer(GL_FRONT);
        glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, pixel_data);

//        fwrite(&TGAhead,sizeof(unsigned char),8,out);
        fwrite(&TGAhead,sizeof(TGAhead),1,out);
        fwrite(pixel_data, 3*W*H, 1, out);
        fclose(out);
}
void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(float i = 0; i<width-1; i+=1){
        for(float j = 0; j<width-1; j+=1){
    glBegin(GL_POLYGON);
        Mass m1 = *masses[i*width+j];
        Mass m2 = *masses[i*width+j+1];
        Mass m3 = *masses[(i+1)*width+j+1];
        Mass m4 = *masses[(i+1)*width+j];
        Eigen::Vector3f m11;
        Eigen::Vector3f m12;
        Eigen::Vector3f m13;
        Eigen::Vector3f m14;
        Eigen::Vector3f n;
        m11 << m1.position.x(),m1.position.y(),m1.position.z();
        m12 << m2.position.x(),m2.position.y(),m2.position.z();
        m14 << m4.position.x(),m4.position.y(),m4.position.z();
        n = (m11-m12).cross(m11-m14);
        n = n/n.norm();
        glNormal3f(n.x(),n.y(),n.z());
        glVertex3f(m1.position.x(),m1.position.y(),m1.position.z());
        glNormal3f(n.x(),n.y(),n.z());
        glVertex3f(m2.position.x(),m2.position.y(),m2.position.z());
        glNormal3f(n.x(),n.y(),n.z());
        glVertex3f(m4.position.x(),m4.position.y(),m4.position.z());
    glEnd();
    glBegin(GL_POLYGON);
        m12 << m2.position.x(),m2.position.y(),m2.position.z();
        m13 << m3.position.x(),m3.position.y(),m3.position.z();
        m14 << m4.position.x(),m4.position.y(),m4.position.z();
        n = -(m12-m13).cross(m12-m14);
        n = n/n.norm();
        glNormal3f(n.x(),n.y(),n.z());
        glVertex3f(m2.position.x(),m2.position.y(),m2.position.z());
        glNormal3f(n.x(),n.y(),n.z());
        glVertex3f(m3.position.x(),m3.position.y(),m3.position.z());
        glNormal3f(n.x(),n.y(),n.z());
        glVertex3f(m4.position.x(),m4.position.y(),m4.position.z());
    glEnd();
        }
    }

   glFlush ();
}
void update(){
        updateCount++;
       if(updateCount%180==0)
       {
        screendump(imageCount,HEIGHT,WIDTH);
        imageCount++;
       }
        if(updateCount==2000){
    //masses[0]->pinned = 0;
    //masses[width-1]->pinned = 0;
    //masses[width*width-1]->pinned = 0;
    //masses[width*(width-1)]->pinned = 0;
    masses[(int)((width*.5)*width+(width*.5))]->pinned = 0;
        }
    for(int i = 0; i<masses.size(); i++){
        Mass *m = masses[i];
        if(updateCount){
                m->pinned = 0;
        }
        m->update(.03f);
        float damp = .8f;
        if(m->position.x()<-2+r)
                m->velocity.x() = (damp*abs(m->velocity.x()));
        if(m->position.x()>2-r)
                m->velocity.x() = -(damp*abs(m->velocity.x()));
        if(m->position.y()<-2+r)
                m->velocity.y() = (damp*abs(m->velocity.y()));
        if(m->position.y()>2-r)
                m->velocity.y() = -(damp*abs(m->velocity.y()));
        if(m->position.z()<-2+r)
                m->velocity.z() = (damp*abs(m->velocity.z()));
        if(m->position.z()>2-r)
                m->velocity.z() = -(damp*abs(m->velocity.z()));
    }
    for(int i = 0; i<masses.size(); i++){
        Mass *m = masses[i];
        m->commit();
    }
    display();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
    gluPerspective(65.0,1.0,1,100);
    
  /* if (w <= h)
      glFrustum (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
         1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glFrustum (-1.5*(GLfloat)w/(GLfloat)h,
         1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
    */
    gluLookAt(0,3,0,
              0,0,-1,
              0,0,1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
    int index = 0;
    float step = 2.0f/width;
    Sphere s;
    s.radius = .2f;
    s.position = Eigen::Vector4f();
    s.position << 0,0,-.4,1;
    for(float i = -1; i<1-step+.0002; i+=step){
        for(float j = -1; j<1-step+.0002; j+=step){
        Mass *m = new Mass();
        m->objects.push_back((Object*)(&s));
        Eigen::Vector4f position;
        Eigen::Vector4f velocity;
        if(i==-1)
        {
                m->pinned = 1;
                //position << i,j,0,1;
        }
        //else
            position << i,j,rand()*.03f/(float)RAND_MAX-.2f,1;
        //    position << i,j,-.2f,1;
        velocity << 0,0,0,0;
        //velocity << rand()*.10f/(float)RAND_MAX-1,rand()*.10f/(float)RAND_MAX-1,rand()*.10f/(float)RAND_MAX-1,0;
        m->position = position;
        m->velocity = velocity;
        masses.push_back(m);

        /*if(index%2==1){
            m->links.push_back(masses[index-1]);
            masses[index-1]->links.push_back(m);
        }*/


        index++;
        }
    }
    masses[0]->pinned = 1;
    masses[width-1]->pinned = 1;
    masses[width*width-1]->pinned = 1;
    masses[width*(width-1)]->pinned = 1;
//    masses[(int)((width*.5)*width+(width*.5))]->pinned = 1;
    for(float i = 0; i<width; i+=1){
        for(float j = 0; j<width; j+=1){
                if(i==0){
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j]);
                }
                else if(i==width-1){
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j]);
                }
                else
                {
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j]);
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j]);
                }

                if(j==0){
                    masses[i*width+j]->links.push_back(masses[i*width+j+1]);
                }
                else if(j==width-1){
                    masses[i*width+j]->links.push_back(masses[i*width+j-1]);
                }
                else
                {
                    masses[i*width+j]->links.push_back(masses[i*width+j+1]);
                    masses[i*width+j]->links.push_back(masses[i*width+j-1]);
                }

                if(j<1 && i>0 && i<width-1){
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j+1]);
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j+1]);
                }
                if(i<1 && j>0 && j<width-1){
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j+1]);
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j-1]);
                }
                if(j>width-1 && i>0 && i<width-1){
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j-1]);
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j-1]);
                }
                if(i>width-1 && j>0 && j<width-1){
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j+1]);
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j-1]);
                }
                if(i<1 && j<1){
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j+1]);
                }
                if(i>width-1 && j>width-1){
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j-1]);
                }
                if(i>0 && i<width-1 && j>0 && j<width-1){
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j+1]);
                    masses[i*width+j]->links.push_back(masses[(i-1)*width+j-1]);
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j+1]);
                    masses[i*width+j]->links.push_back(masses[(i+1)*width+j-1]);
                }
        }
    }

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitDisplayMode(24);
   glutInitWindowSize (HEIGHT, WIDTH); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(update);

   glutMainLoop();
   return 0;
}
