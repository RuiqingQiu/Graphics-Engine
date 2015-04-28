//
//  main.cpp
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/21/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <OpenGL/gl3.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Model3D.h"

#include "Light.h"
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
using std::ifstream;
#include <sstream>
#include <string>
#include "Matrix4.h"
using std::string;
#include "glm/glm.hpp"
using glm::mat4;

#include "glm/gtc/matrix_transform.hpp"
using glm::vec3;


int width = 512;
int height = 512;
int angle = 0;
//#include "Matrix4.h"
Model3D* object;
Light* light;

GLuint vboHandles[2];
GLuint vaoHandle;
GLuint programHandle;
void initialize(void)
{
    /*object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/basic.vert",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/basic.frag");*/
    
    /*object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert",
                         "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");*/

    
    object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/mace.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/normals.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/gloss_mace.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/metallic.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/hammer.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_normals.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_gloss.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_metallic.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
    /*object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj",
     "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG",
     "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png",
     "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG",
     "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG",
     "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/new.vert",
     "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/new.frag");*/
    light = new Light();
    light->setLightPosition(0, 10, 0);
}

void displayCallback()
{
    
    angle = (angle + 1) % 360;
    //Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Set the OpenGL matrix mode to ModelView
    glMatrixMode(GL_MODELVIEW);
    light->updateLight();
    //Push a matrix save point
    //This will save a copy of the current matrix so that we can
    //make changes to it and 'pop' those changes off later.
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, -5);
    glRotatef(angle, 0, 1, 0);

 
    //glutSolidTeapot(5);
    if(object){
        object->x = light->x;
        object->y = light->y;
        object->z = light->z;
        object->OnDraw();
    }
    glPopMatrix();
    glFlush();
    
    //Swap the off-screen buffer (the one we just drew to) with the on-screen buffer
    glutSwapBuffers();
    
}

//----------------------------------------------------------------------------
// Callback method called when system is idle.
// This is called at the start of every new "frame" (qualitatively)
void idleCallback()
{
    //Call the display routine to draw the cube
    displayCallback();
}

//----------------------------------------------------------------------------
// Callback method called by GLUT when graphics window is resized by the user
void reshapeCallback(int w, int h)
{
    width = w;                                                       //Set the window width
    height = h;                                                      //Set the window height
    glViewport(0, 0, w, h);                                          //Set new viewport size
    glMatrixMode(GL_PROJECTION);                                     //Set the OpenGL matrix mode to Projection
    glLoadIdentity();                                                //Clear the projection matrix by loading the identity
    gluPerspective(60.0, double(width)/(double)height, 1.0, 1000.0); //Set perspective projection viewing frustum
    glTranslatef(0, 0, 0);
    glMatrixMode(GL_MODELVIEW);

}
void processNormalKeys(unsigned char key, int x, int y){
    if (key == 27){
        exit(0);
    }
}



int main(int argc, char *argv[])
{
    //GLUT and OpenGL Configuration
    glutInit(&argc, argv);                                      //Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   //Open an OpenGL context with double buffering, RGB colors, and depth buffering
    glutInitWindowSize(512, 512);          //Set initial window size
    glutCreateWindow("Graphics Engine"); //Open window and set window title
    glEnable(GL_DEPTH_TEST);                        //Enable depth buffering
    glClear(GL_DEPTH_BUFFER_BIT);                               //Clear depth buffer
    glClearColor(0.0, 0.0, 0.0, 0.0);                           //Set clear color to black
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                  //Set polygon drawing mode to fill front and back of each polygon
    glDisable(GL_CULL_FACE);                                    //Disable backface culling to render both sides of polygons
    glShadeModel(GL_SMOOTH);                                    //Set shading to smooth
    
    glEnable(GL_COLOR_MATERIAL);                                //Enable color materials
    //glEnable(GL_LIGHTING);                                      //Enable lighting
    glEnable(GL_NORMALIZE);                                     //Auto normalize surface normals
    
    //Register callback functions:
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutKeyboardFunc(processNormalKeys);
    
    initialize();
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    printf("vendor %s\n", vendor);
    printf("renderer %s\n", renderer);
    printf("version %s\n", version);
    printf("glsl %s\n", glslVersion);

    //Start up the render loop!
    glutMainLoop();
    
    return 0;
}