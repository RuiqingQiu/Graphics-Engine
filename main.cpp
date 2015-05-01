
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
#include "SOIL.h"
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

#include "Globals.h"

int width = 512;
int height = 512;
int angle = 0;
int angle_z = 0;
//#include "Matrix4.h"
Model3D* object;
Light* light;


void init();
void display();

GLuint fboHandle;
GLuint renderTex;
GLuint depthBuf;

GLuint colorTex, depthTex;
void getSkybox(int);

GLhandleARB loadShader(char* filename, unsigned int type)
{
    FILE *pfile;
    GLhandleARB handle;
    const GLcharARB* files[1];
    
    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength;
    char* errorLogText;
    GLsizei actualErrorLogLength;
    
    char buffer[400000];
    memset(buffer, 0, 400000);
    
    // This will raise a warning on MS compiler
    pfile = fopen(filename, "rb");
    if (!pfile)
    {
        printf("Sorry, can't open file: '%s'.\n", filename);
        exit(0);
    }
    
    fread(buffer, sizeof(char), 400000, pfile);
    //printf("%s\n",buffer);
    
    
    fclose(pfile);
    
    handle = glCreateShaderObjectARB(type);
    if (!handle)
    {
        //We have failed creating the vertex shader object.
        printf("Failed creating vertex shader object from file: %s.", filename);
        exit(0);
    }
    
    files[0] = (const GLcharARB*)buffer;
    glShaderSourceARB(
                      handle, //The handle to our shader
                      1, //The number of files.
                      files, //An array of const char * data, which represents the source code of theshaders
                      NULL);
    
    glCompileShaderARB(handle);
    
    //Compilation checking.
    glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);
    
    // If an error was detected.
    if (!result)
    {
        //We failed to compile.
        printf("Shader '%s' failed compilation.\n", filename);
        
        //Attempt to get the length of our error log.
        glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
        
        //Create a buffer to read compilation error message
        errorLogText = (char*)malloc(sizeof(char)* errorLoglength);
        
        //Used to get the final length of the log.
        glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);
        
        // Display errors.
        printf("%s\n", errorLogText);
        
        // Free the buffer malloced earlier
        free(errorLogText);
    }
    printf("Shader loaded done\n");
    return handle;
}


void setupFBO()
{
    //    // generate a framebuffer
    //    glGenFramebuffers(1, &fboHandle);
    //    // bind it as the target for rendering commands
    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHandle);
    
    glGenRenderbuffers(1, &fboHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, fboHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboHandle);
    
    
    
    // create a RGBA color texture
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width, height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 NULL);
    
    // create a depth texture
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    
    // create the framebuffer object
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHandle);
    
    // attach color
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTex, 0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,  GL_DEPTH_ATTACHMENT, depthTex, 0);
    
    // check if everything is OK
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHandle);
    GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (e != GL_FRAMEBUFFER_COMPLETE){
        printf("There is a problem with the FBO\n");
    }
    else{
        printf("FBO attach complete\n");
    }
    /*
     // bind the framebuffer as the output framebuffer
     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
     
     // define the index array for the outputs
     GLuint attachments[2] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
     glDrawBuffers(2,  attachments);
     */
    //    glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT, 512);
    //    glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, 512);
    //    glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 4);
    
    
    //    //GLfloat borderColor[4] = {0,0,0,0};
    //    GLuint depthTextureId;
    //    GLenum FBOstatus;
    //
    //    // Try to use a texture depth component
    //    glGenTextures(1, &depthTextureId);
    //    glBindTexture(GL_TEXTURE_2D, depthTextureId);
    //
    //    // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    //    // Remove artefact on the edges of the shadowmap
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    //    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    //
    //    //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
    //
    //
    //
    //    // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
    //    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 512, 512, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //
    //    // create a framebuffer object
    //    glGenFramebuffersEXT(1, &fboHandle);
    //    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboHandle);
    //
    //    // Instruct openGL that we won't bind a color texture with the currently binded FBO
    //    glDrawBuffer(GL_NONE);
    //    glReadBuffer(GL_NONE);
    //
    //    // attach the texture to FBO depth attachment point
    //    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);
    //
    //    // check FBO status
    //    FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    //    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    //        printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
    //    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //    if( result == GL_FRAMEBUFFER_COMPLETE) {
    //        cout << "Framebuffer is complete" << endl;
    //    } else {
    //        switch(result){
    //            case GL_FRAMEBUFFER_UNSUPPORTED:
    //                cout << "UNSUPPORTED" << endl;
    //                break;
    //            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    //                cout << "INCOMPLETE ATTACHMENT" << endl;
    //                break;
    //            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    //                cout << "INCOMPLETE DRAW BUFFER" << endl;
    //                break;
    //            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    //                cout <<"INCOMPLETE READ BUFFER"<< endl;
    //                break;
    //        }
    //        cout << "Framebuffer error: " << result << endl;
    //    }
    //
    //    // switch back to window-system-provided framebuffer
    //    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    
    
    // Generate and bind the framebuffer
    //    glGenFramebuffers(1, &fboHandle);
    //    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    
    //    // Create the texture object
    //    GLuint renderTex;
    //    glGenTextures(1, &renderTex);
    //    glActiveTexture(GL_TEXTURE0);  // Use texture unit 0
    //    glBindTexture(GL_TEXTURE_2D, renderTex);
    //    glTexImage2D( GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    //    // Bind the texture to the FBO
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
    //
    //    // Create the depth buffer
    //    GLuint depthBuf;
    //    glGenRenderbuffers(1, &depthBuf);
    //    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    //    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
    //
    //    // Bind the depth buffer to the FBO
    //    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    //                              GL_RENDERBUFFER, depthBuf);
    //
    //    // Set the targets for the fragment output variables
    //    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    //    glDrawBuffers(1, drawBuffers);
    //
    //    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //    if( result == GL_FRAMEBUFFER_COMPLETE) {
    //        cout << "Framebuffer is complete" << endl;
    //    } else {
    //        switch(result){
    //            case GL_FRAMEBUFFER_UNSUPPORTED:
    //            cout << "UNSUPPORTED" << endl;
    //            break;
    //    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    //                cout << "INCOMPLETE ATTACHMENT" << endl;
    //        break;
    //    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    //                cout << "INCOMPLETE DRAW BUFFER" << endl;
    //        break;
    //    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    //                cout <<"INCOMPLETE READ BUFFER"<< endl;
    //                break;
    //        }
    //        cout << "Framebuffer error: " << result << endl;
    //    }
    //
    //    // Unbind the framebuffer, and revert to default framebuffer
    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//First time render
void pass1()
{
    
    //    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 1);
    //    glPushMatrix();
    //    glTranslatef(0, 0, 5);
    //    glutSolidTeapot(1);
    //    glPopMatrix();
    
    // bind the framebuffer as the output framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHandle);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Set Drawing buffers
    GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  attachments);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0, 0, 5);
    glutSolidTeapot(1);
    glPopMatrix();
    
    // get back to the default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

//Second time render
void pass2()
{
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "RenderTex"), 0);
    
    // DRAW PLANE WITH TEXTURE FROM FBO
    // render a quad on the left size of the window
    // with the framebuffer object result
    // set the viewport to the left half of the window
    glPushMatrix();
    glTranslatef(0, 0, 5);
    glutSolidTeapot(1);
    glPopMatrix();
    
    // load identity matrices for VIEW and MODEL
    
    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //    GLuint whiteTexHandle;
    //    GLubyte whiteTex[] = {0, 255, 255, 255};
    //    glActiveTexture(GL_TEXTURE1);
    //    glGenTextures(1, &whiteTexHandle);
    //    glBindTexture(GL_TEXTURE_2D, whiteTexHandle);
    //    glTexImage2D( GL_TEXTURE_2D, 1, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA,GL_UNSIGNED_BYTE ,whiteTex);
    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, renderTex);
    //    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 2);
    //    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "RenderTex"), 0);
    //    glPushMatrix();
    //    glTranslatef(0, 0, 5);
    //    glutSolidTeapot(1);
    //    glPopMatrix();
    //
    //    /*
    //    glUseProgramObjectARB(0);
    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();
    //    glOrtho(-width/2,width/2,-height/2,height/2,1,20);
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();
    //    glColor4f(1,1,1,1);
    //    glActiveTextureARB(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D,depthBuf);
    //    glEnable(GL_TEXTURE_2D);
    //    glTranslated(0,0,-1);
    //    glBegin(GL_QUADS);
    //    glTexCoord2d(0,0);glVertex3f(0,0,0);
    //    glTexCoord2d(1,0);glVertex3f(width/2,0,0);
    //    glTexCoord2d(1,1);glVertex3f(width/2,height/2,0);
    //    glTexCoord2d(0,1);glVertex3f(0,height/2,0);
    //     */
    //
    //    glEnd();
    //    glDisable(GL_TEXTURE_2D);
    
}

void initialize(void)
{
    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/basic.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/basic.frag");
    
    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG");
    
    
    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/mace.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/normals.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/gloss_mace.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/metallic.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
    
    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/woodcube.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_normal.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_gloss.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_metallic.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
    
    
    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/hammer.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_normals.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_gloss.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_metallic.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
    light = new Light();
    light->setLightPosition(0, 10, 0);
    setupFBO();
}

vector<const GLchar*> faces;

GLuint loadCubemap(vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    
    int width,height;
    unsigned char* image;
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(
                     GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
                     );
    }
    /*
     GLubyte red[3] = {255, 0, 0};
     GLubyte green[3] = {0, 255, 0};
     GLubyte blue[3] = {0, 0, 255};
     GLubyte cyan[3] = {0, 255, 255};
     GLubyte magenta[3] = {255, 0, 255};
     GLubyte yellow[3] = {255, 255, 0};
     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X ,
     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, red);
     glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X ,
     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, green);
     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y ,
     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, blue);
     glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ,
     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, cyan);
     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z ,
     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, magenta);
     glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ,
     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, yellow);*/
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return textureID;
}

void displayCallback()
{
    
    //angle = (angle + 1) % 360;
    //Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Set the OpenGL matrix mode to ModelView
    glMatrixMode(GL_MODELVIEW);
    //light->updateLight();
    //Push a matrix save point
    //This will save a copy of the current matrix so that we can
    //make changes to it and 'pop' those changes off later.
    //glUseProgramObjectARB(reflection_shader);
    
    
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
    glRotated(angle, 0, 1, 0);
    
    /*
     int size_of_texture_cube = 100;
     
     
     glUseProgramObjectARB(Globals::skybox_shader);
     glBegin(GL_QUADS);
     glNormal3f(0, 0, -1);
     glVertex3f( size_of_texture_cube,-size_of_texture_cube,size_of_texture_cube );
     glVertex3f(-size_of_texture_cube,-size_of_texture_cube,size_of_texture_cube);
     glVertex3f(-size_of_texture_cube, size_of_texture_cube,size_of_texture_cube); //back up right
     glVertex3f(size_of_texture_cube,size_of_texture_cube, size_of_texture_cube ); //back up left
     glEnd();
     
     //Front[0]
     
     
     glBegin(GL_QUADS);
     glNormal3f(0, 0, 1);
     glVertex3f(  -size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
     glVertex3f( size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
     glVertex3f( size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube);   //up right
     glVertex3f(  -size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube); //up left
     glEnd();
     
     
     glBegin(GL_QUADS);
     glNormal3f(-1, 0, 0);
     glVertex3f(  -size_of_texture_cube, -size_of_texture_cube, size_of_texture_cube);
     glVertex3f( -size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
     glVertex3f( -size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube); //up
     glVertex3f(  -size_of_texture_cube, size_of_texture_cube, size_of_texture_cube); //up
     glEnd();
     
     
     
     glBegin(GL_QUADS);
     glNormal3f(1, 0, 0);
     glVertex3f( size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
     glVertex3f( size_of_texture_cube, -size_of_texture_cube, size_of_texture_cube);
     glVertex3f( size_of_texture_cube, size_of_texture_cube, size_of_texture_cube);
     glVertex3f( size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube);
     glEnd();
     
     glBegin(GL_QUADS);
     glNormal3f(0.0, 1.0, 0.0);
     glVertex3f( -size_of_texture_cube, size_of_texture_cube, size_of_texture_cube); //connect to back up left
     glVertex3f( -size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube); //connect to front up left
     
     glVertex3f( size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube);
     glVertex3f( size_of_texture_cube, size_of_texture_cube, size_of_texture_cube);
     glEnd();
     
     
     if(object){
     object->OnDraw();
     }
     */
    //glutSolidSphere(1, 20, 20);
    //glUseProgramObjectARB(0);
    //glutSolidTeapot(1);
    
    glUseProgramObjectARB(Globals::edge_shader);
    pass1();
    pass2();
    glPopMatrix();
    
    glFlush();
    //Swap the off-screen buffer (the one we just drew to) with the on-screen buffer
    glutSwapBuffers();
    glUseProgramObjectARB(0);
    
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
    glTranslatef(0, 0, -10);
    glMatrixMode(GL_MODELVIEW);
    
}
void processNormalKeys(unsigned char key, int x, int y){
    if (key == 27){
        exit(0);
    }
    if(key == 'r'){
        angle = (angle + 1) % 360;
    }
    else if(key == 'v'){
        angle_z = (angle_z + 1) % 360;
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
    //glutDisplayFunc(displayCallback);
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeCallback);
    //glutIdleFunc(idleCallback);
    glutIdleFunc(glutPostRedisplay);
    glutKeyboardFunc(processNormalKeys);
    
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    
    printf("vendor %s\n", vendor);
    printf("renderer %s\n", renderer);
    printf("version %s\n", version);
    printf("glsl %s\n", glslVersion);
    
    //Basic light shader
    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;
    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/multiple_light.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/multiple_light.frag", GL_FRAGMENT_SHADER);
    Globals::light_shader = glCreateProgramObjectARB();
    glAttachObjectARB(Globals::light_shader, vertexShaderHandle);
    glAttachObjectARB(Globals::light_shader, fragmentShaderHandle);
    glLinkProgramARB(Globals::light_shader);
    
    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/edge.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/edge.frag", GL_FRAGMENT_SHADER);
    Globals::edge_shader = glCreateProgramObjectARB();
    glAttachObjectARB(Globals::edge_shader, vertexShaderHandle);
    glAttachObjectARB(Globals::edge_shader, fragmentShaderHandle);
    glLinkProgramARB(Globals::edge_shader);
    
    
    
    //Reflection shader
    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/reflect.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/reflect.frag", GL_FRAGMENT_SHADER);
    Globals::reflection_shader = glCreateProgramObjectARB();
    glAttachObjectARB(Globals::reflection_shader, vertexShaderHandle);
    glAttachObjectARB(Globals::reflection_shader, fragmentShaderHandle);
    glLinkProgramARB(Globals::reflection_shader);
    
    
    //Refraction shader
    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/refract.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/refract.frag", GL_FRAGMENT_SHADER);
    Globals::refraction_shader = glCreateProgramObjectARB();
    glAttachObjectARB(Globals::refraction_shader, vertexShaderHandle);
    glAttachObjectARB(Globals::refraction_shader, fragmentShaderHandle);
    glLinkProgramARB(Globals::refraction_shader);
    
    //Skybox shader
    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/skybox.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/skybox.frag", GL_FRAGMENT_SHADER);
    Globals::skybox_shader = glCreateProgramObjectARB();
    glAttachObjectARB(Globals::skybox_shader, vertexShaderHandle);
    glAttachObjectARB(Globals::skybox_shader, fragmentShaderHandle);
    glLinkProgramARB(Globals::skybox_shader);
    
    
    faces.clear();
    getSkybox(CLOUDS);
    GLuint cubemapTexture = loadCubemap(faces);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    
    //glUniform1i(glGetUniformLocationARB(shader_id, "tex"), 0);
    std::cout << glGetError() << std::endl;
    //Start up the render loop!
    
    //setupFBO();
    //initialize();
    init();
    glutMainLoop();
    
    return 0;
}

void getSkybox(int type){
    switch(type){
        case ALIEN:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_front.jpg");
            break;
            
        case BLUE_SKY:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_front.jpg");
            break;
        case BLUE_GREEN:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_front.jpg");
            break;
        case CALM:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_front.jpg");
            break;
        case CLOUDS:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_front.jpg");
            break;
        case EMERALD:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_front.jpg");
            break;
        case EMERALD_FOG:
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_left.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_right.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_top.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_back.jpg");
            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_front.jpg");
            break;
    }
}

GLuint fb, color, depth;


void CHECK_FRAMEBUFFER_STATUS()
{
    GLenum status;
    status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
            
        case GL_FRAMEBUFFER_UNSUPPORTED:
            /* choose different formats */
            break;
            
        default:
            /* programming error; will fail on all hardware */
            fputs("Framebuffer Error\n", stderr);
            exit(-1);
    }
}

float const light_dir[]={1,1,1,0};
float const light_color[]={1,0.95,0.9,1};

void init()
{
    glGenFramebuffers(1, &fb);
    glGenTextures(1, &color);
    glGenRenderbuffers(1, &depth);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(	GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width, height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
    
    CHECK_FRAMEBUFFER_STATUS();
}

void prepare()
{
    angle = (angle + 1) % 360;
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glViewport(0,0, width, height);
    
    glClearColor(0.2,0.2,1,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 10);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
//    glEnable(GL_LIGHT0);
//    glEnable(GL_LIGHTING);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
//    glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    
    glUseProgramObjectARB(Globals::edge_shader);
    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 1);

    glTranslatef(0,0,-5);
    glRotatef(angle, 0, 1, 0);
    //glColor3f(1, 0, 0);
    glutSolidTeapot(1);

    //glutSolidTeapot(1);
    //glutSolidTeapot(0.75);
    glUseProgramObjectARB(0);
}

void final()
{
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glViewport(0,0, width, height);
    
    glClearColor(1.,1.,1.,0.);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 10);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-5);
    glRotatef(angle, 0, 1, 0);
    
    GLuint texture;
    glBindTexture(GL_TEXTURE_2D, texture);
    texture = SOIL_load_OGL_texture("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_tex.png" , SOIL_LOAD_AUTO,
                                         SOIL_CREATE_NEW_ID,
                                         SOIL_FLAG_INVERT_Y);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //SOIL_free_image_data(image);
    if (texture == 0)
    {
        cout << "error 1" << endl;
    }

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texture);

    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, color);
    // Draw a textured quad
//    glBegin(GL_QUADS);
//    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
//    glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
//    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
//    glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
//    glEnd();
//    
//    
//    glDisable(GL_TEXTURE_2D);
    
    glUseProgramObjectARB(Globals::edge_shader);
    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 2);
    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "RenderTex"), 0);
    //glColor3f(0, 1, 0);
    glutSolidTeapot(1);
    
    /*
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDisable(GL_LIGHTING);
    
    float cube[][5]=
    {
        {-1, -1, -1,  0,  0},
        { 1, -1, -1,  1,  0},
        { 1,  1, -1,  1,  1},
        {-1,  1, -1,  0,  1},
        
        {-1, -1,  1, -1,  0},
        { 1, -1,  1,  0,  0},
        { 1,  1,  1,  0,  1},
        {-1,  1,  1, -1,  1},
    };
    unsigned int faces[]=
    {
        0, 1, 2, 3,
        1, 5, 6, 2,
        5, 4, 7, 6,
        4, 0, 3, 7,
        3, 2, 6, 7,
        4, 5, 1, 0
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 5*sizeof(float), &cube[0][0]);
    glTexCoordPointer(2, GL_FLOAT, 5*sizeof(float), &cube[0][3]);
    
    //glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, faces);
    
    glCullFace(GL_FRONT);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, faces);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    */
}

void display()
{
    prepare();
    final();
    
    glutSwapBuffers();
}
