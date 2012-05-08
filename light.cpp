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
float r = .005f;
int width = 40;
int updateCount = 0;
int imageCount = 0;
char *outputDir;
Sphere s;
Sphere s2;
   GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 0.0 };
   GLfloat mat_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_diffuse[] = { 0.0, 0.5f, 0.5, 1.0 };
   GLfloat mat_diffuse2[] = { 0.5, 0.0, 0.5, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
GLuint LoadTextureRAW( const char * filename)
{
        GLuint texture;
        // allocate a texture name
        glGenTextures( 1, &texture );
        // select our current texture
        glBindTexture( GL_TEXTURE_2D, texture );
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        // when texture area is large, bilinear filter the original
        // when texture area is small, bilinear filter the closest mipmap
       glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                         GL_LINEAR_MIPMAP_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        int wrap = 1;
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                             wrap ? GL_REPEAT : GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                                 wrap ? GL_REPEAT : GL_CLAMP );

        int width, height;
        char * data;
        FILE * file;
        // texture data
        width = 256;
        height = 256;

        // allocate buffer
        data = (char *)malloc( width * height * 3 );

        // open and read texture data
        file = fopen( filename, "rb" );
        fread( data, width * height * 3, 1, file );
        fclose( file );

        // build our texture mipmaps
        gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                                           GL_RGB, GL_UNSIGNED_BYTE, data );

        // free buffer
        free( data );

        return texture;
}
void init(void) 
{
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_TEXTURE_2D );

   GLfloat light_position0[] = { -4.0, 4.0f, 2.0f, 0.0 };
   GLfloat light_position1[] = { 4.0, 4.0f, 2.0f, 0.0 };
   GLfloat light_position2[] = { 4.0, -4.0f, 2.0f, 0.0 };
   GLfloat light_position3[] = { -4.0, -4.0f, 2.0f, 0.0 };
    GLfloat front_mat_diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

 //  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, front_mat_diffuse);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, front_mat_diffuse);
    glLightfv(GL_LIGHT3, GL_POSITION, light_position3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, front_mat_diffuse);

    //glBindTexture( GL_TEXTURE_2D, LoadTextureRAW("texture.RAW"));
}
void screendump(int index, int W, int H) {
        string dir = outputDir;
        string name1 = "/screenshot";
        string name2 = ".tga";
        string name3;
        std::stringstream namestr;
        namestr << index;
        name3 = namestr.str();
        string name = dir + name1 + name3 + name2;

        FILE   *out = fopen((char*)name.c_str(),"wb");
        char   *pixel_data = new char[3*W*H];
        short  TGAhead[] = { 0, 2, 0, 0, 0, 0, W, H, 24 };

        glReadBuffer(GL_FRONT);
        glReadPixels(0, 0, W, H, GL_BGR, GL_UNSIGNED_BYTE, pixel_data);

        fwrite(&TGAhead,sizeof(TGAhead),1,out);
        fwrite(pixel_data, 3*W*H, 1, out);
        fclose(out);
}
void cross(Mass* m, Mass* m1, Mass* m2, Eigen::Vector3f *vector){
        Eigen::Vector3f n1;
        n1 << (m->position-m1->position).x(),(m->position-m1->position).y(),(m->position-m1->position).z();
        Eigen::Vector3f n2;
        n2 << (m->position-m2->position).x(),(m->position-m2->position).y(),(m->position-m2->position).z();
        *vector = (n1.cross(n2)).normalized();
}
void normalOf(int i,int j, Eigen::Vector3f *normal){
    Eigen::Vector3f n;
    n << 0,0,0;
           Eigen::Vector3f v;
           v << 0,0,0;
    if(i>0 && i<width-1 && j>0 && j<width-1){
           cross(masses[i*width+j],masses[(i+1)*width+j],masses[i*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i)*width+j+1],masses[(i-1)*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i-1)*width+j+1],masses[(i-1)*width+j],&v);
           n += v;
           cross(masses[i*width+j],masses[(i-1)*width+j],masses[i*width+j-1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i)*width+j-1],masses[(i+1)*width+j-1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i+1)*width+j-1],masses[(i+1)*width+j],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(i==0 && j>0 && j<width-1){
           cross(masses[i*width+j],masses[(i+1)*width+j],masses[i*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i)*width+j-1],masses[(i+1)*width+j-1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i+1)*width+j-1],masses[(i+1)*width+j],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(i==width-1 && j>0 && j<width-1){
           cross(masses[i*width+j],masses[(i)*width+j+1],masses[(i-1)*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i-1)*width+j+1],masses[(i-1)*width+j],&v);
           n += v;
           cross(masses[i*width+j],masses[(i-1)*width+j],masses[i*width+j-1],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(j==0 && i>0 && i<width-1){
           cross(masses[i*width+j],masses[(i+1)*width+j],masses[i*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i)*width+j+1],masses[(i-1)*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i-1)*width+j+1],masses[(i-1)*width+j],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(j==width-1 && i>0 && i<width-1){
           cross(masses[i*width+j],masses[(i-1)*width+j],masses[i*width+j-1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i)*width+j-1],masses[(i+1)*width+j-1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i+1)*width+j-1],masses[(i+1)*width+j],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(j==0 && i==0){
           cross(masses[i*width+j],masses[(i+1)*width+j],masses[i*width+j+1],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(j==0 && i==width-1){
           cross(masses[i*width+j],masses[(i)*width+j+1],masses[(i-1)*width+j+1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i-1)*width+j+1],masses[(i-1)*width+j],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(j==width-1 && i==width-1){
           cross(masses[i*width+j],masses[(i-1)*width+j],masses[i*width+j-1],&v);
           n += v;
           *normal = n.normalized();
    }
    else if(j==width-1 && i==0){
           cross(masses[i*width+j],masses[(i)*width+j-1],masses[(i+1)*width+j-1],&v);
           n += v;
           cross(masses[i*width+j],masses[(i+1)*width+j-1],masses[(i+1)*width+j],&v);
           n += v;
           *normal = n.normalized();
    }
    else{
        *normal << 0,0,0;
    }
}
void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable( GL_TEXTURE_2D );
   glPushMatrix();
   glTranslatef(s2.position.x(),s2.position.y(),s2.position.z());
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse2);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular2);
   glutSolidSphere(s2.radius-.01f,40,24);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(s.position.x(),s.position.y(),s.position.z());
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse2);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular2);
   glutSolidSphere(s.radius-.01f,40,24);
   glPopMatrix();
    glEnable( GL_TEXTURE_2D );

        
    float scale = 1.0f/(2.0*width); 
    float shift = 0.25f;
   glPushMatrix();
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
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
        //n = -(m11-m12).cross(m11-m14);
        //n = n/n.norm();
        normalOf(i,j,&n);
        glNormal3f(n.x(),n.y(),n.z());
       // glTexCoord3f(m1.position.x(),m1.position.y(),m1.position.z());
        glTexCoord3f(i*scale-shift,j*scale-shift,0);
        glVertex3f(m1.position.x(),m1.position.y(),m1.position.z());
        normalOf(i,j+1,&n);
        glNormal3f(n.x(),n.y(),n.z());
        //glTexCoord3f(m2.position.x(),m2.position.y(),m2.position.z());
        glTexCoord3f(i*scale-shift,(j+1)*scale-shift,0);
        glVertex3f(m2.position.x(),m2.position.y(),m2.position.z());
        normalOf(i+1,j,&n);
        glNormal3f(n.x(),n.y(),n.z());
        //glTexCoord3f(m4.position.x(),m4.position.y(),m4.position.z());
        glTexCoord3f((i+1)*scale-shift,j*scale-shift,0);
        glVertex3f(m4.position.x(),m4.position.y(),m4.position.z());
    glEnd();
    glBegin(GL_POLYGON);
        m12 << m2.position.x(),m2.position.y(),m2.position.z();
        m13 << m3.position.x(),m3.position.y(),m3.position.z();
        m14 << m4.position.x(),m4.position.y(),m4.position.z();
        n = -(m12-m13).cross(m12-m14);
        n = n/n.norm();
        normalOf(i,j+1,&n);
        glNormal3f(n.x(),n.y(),n.z());
        //glTexCoord3f(m2.position.x(),m2.position.y(),m2.position.z());
        glTexCoord3f(i*scale-shift,(j+1)*scale-shift,0);
        glVertex3f(m2.position.x(),m2.position.y(),m2.position.z());
        normalOf(i+1,j+1,&n);
        glNormal3f(n.x(),n.y(),n.z());
        //glTexCoord3f(m3.position.x(),m3.position.y(),m3.position.z());
        glTexCoord3f((i+1)*scale-shift,(j+1)*scale-shift,0);
        glVertex3f(m3.position.x(),m3.position.y(),m3.position.z());
        normalOf(i+1,j,&n);
        glNormal3f(n.x(),n.y(),n.z());
        //glTexCoord3f(m4.position.x(),m4.position.y(),m4.position.z());
        glTexCoord3f((i+1)*scale-shift,j*scale-shift,0);
        glVertex3f(m4.position.x(),m4.position.y(),m4.position.z());
    glEnd();
        }
    }
   glPopMatrix();
   glFlush ();
}
void update(){
    if(updateCount%4==0){
        screendump(imageCount,HEIGHT,WIDTH);   
        imageCount++;
    }
    updateCount++;
    for(int i = 0; i<masses.size(); i++){
        Mass *m = masses[i];
        m->update(.03f);
    }
    for(int i = 0; i<masses.size(); i++){
        Mass *m = masses[i];
        m->commit();
    }
    for(int i = 0; i<masses.size(); i++){
        Mass *m = masses[i];
        m->updateSphere(.03f);
    }
    //s.update();
    for(int i = 0; i<masses.size(); i++){
        Mass *m = masses[i];
        m->updateSphere(.03f);
    }
    display();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
    gluPerspective(65.0,1.0,1,100);
    gluLookAt(0,1.8,0,
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
void link(Mass *m1, Mass *m2, float l,float ks){
            Mass::Link l1;
            l1.m = m2;
            l1.length = l;
            l1.k = ks;
            m1->links.push_back(l1);
            Mass::Link l2;
            l2.m = m1;
            l2.length = l;
            l2.k = ks;
            m2->links.push_back(l2);
}
int main(int argc, char** argv)
{
    outputDir = "tmp0";
    float k = 40.0f;
    float kb = k/.4f;
    for(int i = 0; i<argc; i++){
        char *item = argv[i];
        if(strcmp(item,"-outputDir")==0)
                outputDir = argv[i+1]; 
        if(strcmp(item,"-kb")==0)
                kb = k/atof(argv[i+1]); 
    }

    float l = 1.0f/width;
    float ld = 1.4142*l;
    float l2 = l*1.4142*2.0f;
    s.radius = .2f;
    s.position = Eigen::Vector4f();
    s.position << 0.0f,0,-.4,1;
    s2.radius = .3f;
    s2.position = Eigen::Vector4f();
    s2.position << -0.35f,0,.7,1;

    for(int i = 0; i<width; i++){
        for(int j = 0; j<width; j++){
            Mass *m1 = new Mass();
            m1->position << -.5f+i*l,-.5f+j*l,-.2,1;
            m1->objects.push_back((Object*)(&s));
            m1->objects.push_back((Object*)(&s2));
    //        if((j<=3 || j>=width-4) && (i<=3 || i >= width-4))
    //                m1->pinned = 1;
            masses.push_back(m1);
        }
    }
    for(int i = 0; i<width; i++){
        for(int j = 0; j<width; j++){
                if(i==0){
                        if(j==0){
                            link(masses[i*width+j],masses[(i+1)*width+j+1],ld,k);
                        }
                        else if(j==width-1){
                            link(masses[i*width+j],masses[(i+1)*width+j-1],ld,k);
                        }
                        else{
                            link(masses[i*width+j],masses[(i+1)*width+j+1],ld,k);
                            link(masses[i*width+j],masses[(i+1)*width+j-1],ld,k);
                        }
                        link(masses[i*width+j],masses[(i+1)*width+j],l,k);
                }
                else if(i==width-1){
                        if(j==0){
                            link(masses[i*width+j],masses[(i-1)*width+j+1],ld,k);
                        }
                        else if(j==width-1){
                            link(masses[i*width+j],masses[(i-1)*width+j-1],ld,k);
                        }
                        else{
                            link(masses[i*width+j],masses[(i-1)*width+j+1],ld,k);
                            link(masses[i*width+j],masses[(i-1)*width+j-1],ld,k);
                        }
                        link(masses[i*width+j],masses[(i-1)*width+j],l,k);
                }
                else{
                        if(j==0){
                            link(masses[i*width+j],masses[(i+1)*width+j+1],ld,k);
                            link(masses[i*width+j],masses[(i-1)*width+j+1],ld,k);
                        }
                        else if(j==width-1){
                            link(masses[i*width+j],masses[(i+1)*width+j-1],ld,k);
                            link(masses[i*width+j],masses[(i-1)*width+j-1],ld,k);
                        }
                        else{
                            link(masses[i*width+j],masses[(i+1)*width+j+1],ld,k);
                            link(masses[i*width+j],masses[(i+1)*width+j-1],ld,k);
                            link(masses[i*width+j],masses[(i-1)*width+j+1],ld,k);
                            link(masses[i*width+j],masses[(i-1)*width+j-1],ld,k);
                        }
                        link(masses[i*width+j],masses[(i+1)*width+j],l,k);
                        link(masses[i*width+j],masses[(i-1)*width+j],l,k);
                }
                if(j==0){
                        link(masses[i*width+j],masses[i*width+j+1],l,k);
                }
                else if(j==width-1){
                        link(masses[i*width+j],masses[i*width+j-1],l,k);
                }
                else{
                        link(masses[i*width+j],masses[i*width+j+1],l,k);
                        link(masses[i*width+j],masses[i*width+j-1],l,k);
                }
                if(j>1 && j<width-2 && i>1 && i<width-2){
                            link(masses[i*width+j],masses[(i+2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j+2],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j-2],2*l,kb);
                }
                if(j>1 && j<width-2 && i<2 && i<width-2){
                            link(masses[i*width+j],masses[(i+2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j+2],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j-2],2*l,kb);
                }
                if(j>1 && j<width-2 && i>1 && i>width-3){
                            link(masses[i*width+j],masses[(i-2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j+2],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j-2],2*l,kb);
                }
                if(j<2 && j<width-2 && i>1 && i<width-2){
                            link(masses[i*width+j],masses[(i+2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j+2],2*l,kb);
                }
                if(j>1 && j>width-3 && i>1 && i<width-2){
                            link(masses[i*width+j],masses[(i+2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j-2],2*l,kb);
                }
                if(j<2 && i<2){
                            link(masses[i*width+j],masses[(i+2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j+2],2*l,kb);
                }
                if(j<2 && i>width-3){
                            link(masses[i*width+j],masses[(i-2)*width+j+2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j+2],2*l,kb);
                }
                if(j>width-3 && i>width-3){
                            link(masses[i*width+j],masses[(i-2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i-2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j-2],2*l,kb);
                }
                if(j>width-3 && i<2){
                            link(masses[i*width+j],masses[(i+2)*width+j-2],l2,kb);
                            link(masses[i*width+j],masses[(i+2)*width+j],2*l,kb);
                            link(masses[i*width+j],masses[(i)*width+j-2],2*l,kb);
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
