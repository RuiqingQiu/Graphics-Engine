#include "GLUT/glut.h"

#include <stdio.h>
#include <iostream>
#include <math.h>

// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).
#define SHADOW_MAP_RATIO 4

int width = 512;
int height = 512;

//Camera position
float p_camera[3] = {0,5,-20};

//Camera lookAt
float l_camera[3] = {0,0,0};

//Light position
float p_light[3] = {0,20,1};

//Light lookAt
float l_light[3] = {0,0,0};

// Hold id of the framebuffer for light POV rendering
GLuint fboId;

// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;



// Loading shader function
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

void loadShadowShader()
{
    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;
    
    vertexShaderHandle   = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/tmp.vert",GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/tmp.frag",GL_FRAGMENT_SHADER);
    
    shadowShaderId = glCreateProgramObjectARB();
    
    glAttachObjectARB(shadowShaderId,vertexShaderHandle);
    glAttachObjectARB(shadowShaderId,fragmentShaderHandle);
    glLinkProgramARB(shadowShaderId);
    
    shadowMapUniform = glGetUniformLocationARB(shadowShaderId,"ShadowMap");
}

void generateShadowFBO()
{
    int shadowMapWidth = width * SHADOW_MAP_RATIO;
    int shadowMapHeight = height * SHADOW_MAP_RATIO;
    
    //GLfloat borderColor[4] = {0,0,0,0};
    
    GLenum FBOstatus;
    
    // Try to use a texture depth component
    glGenTextures(1, &depthTextureId);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);
    
    // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Remove artefact on the edges of the shadowmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    
    //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
    
    
    
    // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // create a framebuffer object
    glGenFramebuffersEXT(1, &fboId);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
    
    // Instruct openGL that we won't bind a color texture with the currently binded FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // attach the texture to FBO depth attachment point
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);
    
    // check FBO status
    FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
        printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
    
    // switch back to window-system-provided framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupMatrices(float position_x,float position_y,float position_z,float lookAt_x,float lookAt_y,float lookAt_z)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90,float(width)/float(height),1,1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(position_x,position_y,position_z,lookAt_x,lookAt_y,lookAt_z,0,1,0);
}


void setTextureMatrix(void)
{
    static double modelView[16];
    static double projection[16];
    
    // This is matrix transform every coordinate x,y,z
    // x = x* 0.5 + 0.5
    // y = y* 0.5 + 0.5
    // z = z* 0.5 + 0.5
    // Moving from unit cube [-1,1] to [0,1]
    const GLdouble bias[16] = {
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0};
    
    // Grab modelview and transformation matrices
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    
    glMatrixMode(GL_TEXTURE);
    glActiveTextureARB(GL_TEXTURE7);
    
    glLoadIdentity();
    glLoadMatrixd(bias);
    
    // concatating all matrice into one.
    glMultMatrixd (projection);
    glMultMatrixd (modelView);
    
    // Go back to normal matrix mode
    glMatrixMode(GL_MODELVIEW);
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
void startTranslate(float x,float y,float z)
{
    glPushMatrix();
    glTranslatef(x,y,z);
    
    glMatrixMode(GL_TEXTURE);
    glActiveTextureARB(GL_TEXTURE7);
    glPushMatrix();
    glTranslatef(x,y,z);
}

void endTranslate()
{
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawObjects(void)
{
    // Ground
    glColor4f(0.7f,0.7f,0.7f,1);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-35,0,-35);
    glVertex3f(-35,0, 35);
    glVertex3f( 35,0, 35);
    glVertex3f( 35,0,-35);
    glEnd();
    
    glColor4f(0.9f,0.9f,0.9f,1);
    
    // Instead of calling glTranslatef, we need a custom function that also maintain the light matrix
    startTranslate(-2, 1, 3);
    glColor3f(0, 1, 0);
    glutSolidCube(2);
    endTranslate();
    
    startTranslate(2, 1, 5);
    glColor3f(1, 0, 0);
    glutSolidCube(2);
    endTranslate();
    
    for(int i = 0; i < 2; i++){
        startTranslate(i*5 , 1, 10);
        float red = ((float) rand() / (RAND_MAX));
        float green = ((float) rand() / (RAND_MAX));
        float blue = ((float) rand() / (RAND_MAX));

        glColor3f(red, green, blue);
        glutSolidTorus(1.0, 2.0, 20, 20);
        endTranslate();
    }
    
}

void display_shadow(void)
{
    //First step: Render from the light POV to a FBO, story depth values only
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);	//Rendering offscreen
    
    //Using the fixed pipeline to render to the depthbuffer
    glUseProgramObjectARB(0);
    
    // In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
    glViewport(0,0,width * SHADOW_MAP_RATIO,height* SHADOW_MAP_RATIO);
    
    // Clear previous frame values
    glClear( GL_DEPTH_BUFFER_BIT);
    
    //Disable color rendering, we only want to write to the Z-Buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    //setupMatrices(p_light[0],p_light[1],p_light[2],l_light[0],l_light[1],l_light[2]);
    setupMatrices(p_camera[0]+1,p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2]);

    // Culling switching, rendering only backface, this is done to avoid self-shadowing
    glCullFace(GL_FRONT);
    drawObjects();
    
    //Save modelview/projection matrice into texture7, also add a biais
    setTextureMatrix();
    
    
    // Now rendering from the camera POV, using the FBO to generate shadows
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    
    glViewport(0,0,width,height);
    
    //Enabling color write (previously disabled for light POV z-buffer rendering)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    // Clear previous frame values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Using the shadow shader
    glUseProgramObjectARB(shadowShaderId);
    glUniform1iARB(shadowMapUniform,7);
    glUniform3fvARB(glGetUniformLocationARB(shadowShaderId, "LightPosition"), sizeof(p_camera),(float*)p_camera);
    glActiveTextureARB(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D,depthTextureId);
    
    setupMatrices(p_camera[0],p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2]);
    
    glCullFace(GL_BACK);
    drawObjects();
    
    // DEBUG only. this piece of code draw the depth buffer onscreen
    
     glUseProgramObjectARB(0);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-width/2,width/2,-height/2,height/2,1,20);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glColor4f(1,1,1,1);
     glActiveTextureARB(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D,depthTextureId);
     glEnable(GL_TEXTURE_2D);
     glTranslated(0,0,-1);
     glBegin(GL_QUADS);
     glTexCoord2d(0,0);glVertex3f(0,0,0);
     glTexCoord2d(1,0);glVertex3f(width/2,0,0);
     glTexCoord2d(1,1);glVertex3f(width/2,height/2,0);
     glTexCoord2d(0,1);glVertex3f(0,height/2,0);
     
     
     glEnd();
     glDisable(GL_TEXTURE_2D);
    
    glutSwapBuffers();
}

void processNormalKeys_shadow(unsigned char key, int x, int y) {
    
    if (key == 27){
        exit(0);
    }
    else if (key == 'w'){
        p_camera[2] += 1;
        l_camera[2] += 1;
    }
    else if(key == 's'){
        p_camera[2] -= 1;
        l_camera[2] -= 1;
    }
    else if(key == 'a'){
        p_camera[0] -= 1;
        l_camera[0] -= 1;
    }
    else if(key == 'd'){
        p_camera[0] += 1;
        l_camera[0] += 1;
    }
    else if(key == 'u'){
        p_camera[1] += 1;
        l_camera[1] += 1;
    }
    else if(key == 'U'){
        p_camera[1] -= 1;
        l_camera[1] -= 1;
    }
    else if(key == 'z'){
        p_light[0] += 1;
    }
    else if(key == 'Z'){
        p_light[0] -= 1;
    }
    else if(key == 'x'){
        p_light[1] += 1;
    }
    else if(key == 'X'){
        p_light[1] -= 1;
    }
    else if(key == 'c'){
        p_light[2] += 1;
    }
    else if(key == 'C'){
        p_light[2] -= 1;
    }
    display_shadow();

}

void reshape_shadow(int w, int h)
{
    width = w;                                                       //Set the window width
    height = h;                                                      //Set the window height
    glViewport(0, 0, w, h);                                          //Set new viewport size
    glMatrixMode(GL_PROJECTION);                                     //Set the OpenGL matrix mode to Projection
    glLoadIdentity();                                                //Clear the projection matrix by loading the identity
    gluPerspective(90.0, double(width)/(double)height, 1.0, 1000.0); //Set perspective projection viewing frustum
    //glTranslatef(0, 0, -10);
    glMatrixMode(GL_MODELVIEW);
    generateShadowFBO();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(width,height);
    glutCreateWindow("GLSL Shadow mapping");
    
    generateShadowFBO();
    loadShadowShader();
    
    // This is important, if not here, FBO's depthbuffer won't be populated.
    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,1.0f);
    
    glEnable(GL_CULL_FACE);
    
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glutDisplayFunc(display_shadow);
    glutReshapeFunc(reshape_shadow);
    glutKeyboardFunc(processNormalKeys_shadow);
    
    glutMainLoop();
}








//
////
////  main.cpp
////  Graphics Engine
////
////  Created by Ruiqing Qiu on 4/21/15.
////  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
////
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <OpenGL/gl3.h>
//#ifdef __APPLE__
//#include <GLUT/glut.h>
//#else
//#include <GL/glut.h>
//#endif
//
//#include "Model3D.h"
//#include "SOIL.h"
//#include "Light.h"
//#include <cstdio>
//#include <cstdlib>
//
//#include <iostream>
//#include <fstream>
//using std::ifstream;
//#include <sstream>
//#include <string>
//#include "Matrix4.h"
//using std::string;
//#include "glm/glm.hpp"
//using glm::mat4;
//
//#include "glm/gtc/matrix_transform.hpp"
//using glm::vec3;
//
//#include "Globals.h"
//
//int width = 512;
//int height = 512;
//int angle = 0;
//int angle_z = 0;
////#include "Matrix4.h"
//Model3D* object;
//Light* light;
//
//
//void init();
//void display();
//void getSkybox(int);
//
//GLuint fb, color, depth;
//
//
//void CHECK_FRAMEBUFFER_STATUS()
//{
//    GLenum status;
//    status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
//    switch(status) {
//        case GL_FRAMEBUFFER_COMPLETE:
//            break;
//            
//        case GL_FRAMEBUFFER_UNSUPPORTED:
//            /* choose different formats */
//            break;
//            
//        default:
//            /* programming error; will fail on all hardware */
//            fputs("Framebuffer Error\n", stderr);
//            exit(-1);
//    }
//}
//
//float const light_dir[]={1,1,1,0};
//float const light_color[]={1,0.95,0.9,1};
//
//void setupFBO()
//{
//    glGenFramebuffers(1, &fb);
//    glGenTextures(1, &color);
//    glGenRenderbuffers(1, &depth);
//    
//    glBindFramebuffer(GL_FRAMEBUFFER, fb);
//    
//    glBindTexture(GL_TEXTURE_2D, color);
//    glTexImage2D(	GL_TEXTURE_2D,
//                 0,
//                 GL_RGBA,
//                 width, height,
//                 0,
//                 GL_RGBA,
//                 GL_UNSIGNED_BYTE,
//                 NULL);
//    
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
//    
//    glBindRenderbuffer(GL_RENDERBUFFER, depth);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
//    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
//
//    CHECK_FRAMEBUFFER_STATUS();
//    
//    // The depth buffer
//}
//
//GLuint textureId;
//GLuint rboId;
//GLuint fboId;
//
//void setupShadowFBO()
//{
//    // create a texture object
//    /*
//    glGenTextures(1, &textureId);
//    glBindTexture(GL_TEXTURE_2D, textureId);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
//                 GL_RGBA, GL_UNSIGNED_BYTE, 0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    
//    // create a renderbuffer object to store depth info
//    glGenRenderbuffers(1, &rboId);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
//                          width, height);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
//    
//    // create a framebuffer object
//    glGenFramebuffers(1, &fboId);
//    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
//    
//    // attach the texture to FBO color attachment point
//    glFramebufferTexture2D(GL_FRAMEBUFFER,        // 1. fbo target: GL_FRAMEBUFFER
//                           GL_COLOR_ATTACHMENT0,  // 2. attachment point
//                           GL_TEXTURE_2D,         // 3. tex target: GL_TEXTURE_2D
//                           textureId,             // 4. tex ID
//                           0);                    // 5. mipmap level: 0(base)
//    
//    // attach the renderbuffer to depth attachment point
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER,      // 1. fbo target: GL_FRAMEBUFFER
//                              GL_DEPTH_ATTACHMENT, // 2. attachment point
//                              GL_RENDERBUFFER,     // 3. rbo target: GL_RENDERBUFFER
//                              rboId);              // 4. rbo ID
//    
//    // check FBO status
//    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//    if(status != GL_FRAMEBUFFER_COMPLETE)
//        cout << "frame buffer error" << endl;
//    
//    // switch back to window-system-provided framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    */
//}
//
//
//GLhandleARB loadShader(char* filename, unsigned int type)
//{
//    FILE *pfile;
//    GLhandleARB handle;
//    const GLcharARB* files[1];
//    
//    // shader Compilation variable
//    GLint result;				// Compilation code result
//    GLint errorLoglength;
//    char* errorLogText;
//    GLsizei actualErrorLogLength;
//    
//    char buffer[400000];
//    memset(buffer, 0, 400000);
//    
//    // This will raise a warning on MS compiler
//    pfile = fopen(filename, "rb");
//    if (!pfile)
//    {
//        printf("Sorry, can't open file: '%s'.\n", filename);
//        exit(0);
//    }
//    
//    fread(buffer, sizeof(char), 400000, pfile);
//    //printf("%s\n",buffer);
//    
//    
//    fclose(pfile);
//    
//    handle = glCreateShaderObjectARB(type);
//    if (!handle)
//    {
//        //We have failed creating the vertex shader object.
//        printf("Failed creating vertex shader object from file: %s.", filename);
//        exit(0);
//    }
//    
//    files[0] = (const GLcharARB*)buffer;
//    glShaderSourceARB(
//                      handle, //The handle to our shader
//                      1, //The number of files.
//                      files, //An array of const char * data, which represents the source code of theshaders
//                      NULL);
//    
//    glCompileShaderARB(handle);
//    
//    //Compilation checking.
//    glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);
//    
//    // If an error was detected.
//    if (!result)
//    {
//        //We failed to compile.
//        printf("Shader '%s' failed compilation.\n", filename);
//        
//        //Attempt to get the length of our error log.
//        glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
//        
//        //Create a buffer to read compilation error message
//        errorLogText = (char*)malloc(sizeof(char)* errorLoglength);
//        
//        //Used to get the final length of the log.
//        glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);
//        
//        // Display errors.
//        printf("%s\n", errorLogText);
//        
//        // Free the buffer malloced earlier
//        free(errorLogText);
//    }
//    printf("Shader loaded done\n");
//    return handle;
//}
//
//vector<const GLchar*> faces;
//
//GLuint loadCubemap(vector<const GLchar*> faces)
//{
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//    glActiveTexture(GL_TEXTURE0);
//    
//    int width,height;
//    unsigned char* image;
//    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//    
//    for(GLuint i = 0; i < faces.size(); i++)
//    {
//        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
//        glTexImage2D(
//                     GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
//                     GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
//                     );
//    }
//    /*
//     GLubyte red[3] = {255, 0, 0};
//     GLubyte green[3] = {0, 255, 0};
//     GLubyte blue[3] = {0, 0, 255};
//     GLubyte cyan[3] = {0, 255, 255};
//     GLubyte magenta[3] = {255, 0, 255};
//     GLubyte yellow[3] = {255, 255, 0};
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X ,
//     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, red);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X ,
//     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, green);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y ,
//     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, blue);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ,
//     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, cyan);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z ,
//     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, magenta);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ,
//     0,3,1,1,0,GL_RGB,GL_UNSIGNED_BYTE, yellow);*/
//    
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//    
//    return textureID;
//}
//
//
//
//void initialize(void)
//{
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/basic.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/basic.frag");
//    
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Hatchet.obj","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Albedo.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Normal_Clrear.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Gloss.PNG","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Metalness.PNG");
//    
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlackCube/blackcube.obj","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlackCube/tex.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlackCube/normal.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlackCube/gloss.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlackCube/metallic.png");
//    
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/SmallCannon/cannon.obj","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/SmallCannon/tex.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/SmallCannon/normal2.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/SmallCannon/gloss.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/SmallCannon/metall.png");
//    
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/mace.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/normals.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/gloss_mace.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/metallic.png");
//    
//    object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlockyGun/gat.obj","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlockyGun/tex.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlockyGun/normal1.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlockyGun/gloss.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/BlockyGun/metallic.png");
//
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/woodcube.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_normal.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_gloss.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/wooden_cube_metallic.png");
//    
//    
//    //object = new Model3D("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/hammer.obj", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_tex.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_normals.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_gloss.png", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/h_metallic.png","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert","/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
//    light = new Light();
//    light->setLightPosition(0, 10, 0);
//    //setupFBO();
//    //setupShadowFBO();
//}
//
////First pass
//void blur_prepare()
//{
//    //angle = (angle + 1) % 360;
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glEnable(GL_TEXTURE_2D);
//    glBindFramebuffer(GL_FRAMEBUFFER, fb);
//    
//    glViewport(0,0, width, height);
//    
//    glClearColor(1.0,1.0,1,0);
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0, double(width)/(double)height, 0.1, 1000.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
//    
//    
//    glUseProgramObjectARB(Globals::blur_shader);
//    glUniform1i(glGetUniformLocationARB(Globals::blur_shader, "pass"), 1);
//    glUniform1f(glGetUniformLocationARB(Globals::blur_shader, "width"), float(width));
//    glUniform1f(glGetUniformLocationARB(Globals::blur_shader, "height"), float(height));
//    
//    glTranslatef(2,1,-3);
//    glRotatef(angle, 0, 1, 0);
//    //glColor3f(1, 0, 0);
//    //glutSolidTeapot(1);
//    //glutSolidTeapot(1);
//    object->pass = 1;
//    if(object){
//        object->OnDraw();
//    }
//    
//    //glutSolidTeapot(0.75);
//    glUseProgramObjectARB(0);
//}
//
////Second pass
//void blur_final()
//{
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    
//    glViewport(0,0, width, height);
//    
//    glClearColor(1.,1.,1.,0.);
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0, double(width)/(double)height, 0.1, 1000.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslatef(2,1,-3);
//    glRotatef(angle, 0, 1, 0);
//    
//    //glActiveTexture(GL_TEXTURE0);
//    //glBindTexture(GL_TEXTURE_2D, texture);
//    
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, color);
//    // Draw a textured quad
//    glBegin(GL_QUADS);
//    glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
//    glTexCoord2f(0, 1); glVertex3f(1, 2, 0);
//    glTexCoord2f(1, 1); glVertex3f(2, 2, 0);
//    glTexCoord2f(1, 0); glVertex3f(2, 1, 0);
//    glEnd();
//    
//    glDisable(GL_TEXTURE_2D);
//    
//    glUseProgramObjectARB(Globals::blur_shader);
//    glUniform1i(glGetUniformLocationARB(Globals::blur_shader, "pass"), 2);
//    glUniform1f(glGetUniformLocationARB(Globals::blur_shader, "width"), float(width));
//    glUniform1f(glGetUniformLocationARB(Globals::blur_shader, "height"), float(height));
//    //glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "RenderTex"), 0);
//    //glutSolidTeapot(1);
//    object->pass = 2;
//    if(object){
//        object->OnDraw();
//    }
//    glUseProgramObjectARB(0);
//}
//
////First pass
//void prepare()
//{
//    //angle = (angle + 1) % 360;
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glEnable(GL_TEXTURE_2D);
//    glBindFramebuffer(GL_FRAMEBUFFER, fb);
//    
//    glViewport(0,0, width, height);
//    
//    glClearColor(1.0,1.0,1,0);
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0, double(width)/(double)height, 0.1, 1000.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
//    
//    
//    glUseProgramObjectARB(Globals::edge_shader);
//    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 1);
//    glUniform1f(glGetUniformLocationARB(Globals::edge_shader, "width"), float(width));
//    glUniform1f(glGetUniformLocationARB(Globals::edge_shader, "height"), float(height));
//
//    glTranslatef(0,0,-3);
//    glRotatef(angle, 0, 1, 0);
//    //glColor3f(1, 0, 0);
//    glutSolidTeapot(0.1);
//    //glutSolidTeapot(1);
//    object->pass = 1;
//    if(object){
//        object->OnDraw();
//    }
//
//    //glutSolidTeapot(0.75);
//    glUseProgramObjectARB(0);
//}
//
////Second pass
//void final()
//{
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    
//    glViewport(0,0, width, height);
//    
//    glClearColor(1.,1.,1.,0.);
//    
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0, double(width)/(double)height, 0.1, 1000.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslatef(0,0,-3);
//    glRotatef(angle, 0, 1, 0);
//    
//    //glActiveTexture(GL_TEXTURE0);
//    //glBindTexture(GL_TEXTURE_2D, texture);
//    
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, color);
//    // Draw a textured quad
//        glBegin(GL_QUADS);
//        glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
//        glTexCoord2f(0, 1); glVertex3f(1, 2, 0);
//        glTexCoord2f(1, 1); glVertex3f(2, 2, 0);
//        glTexCoord2f(1, 0); glVertex3f(2, 1, 0);
//        glEnd();
//    
//    
//    glDisable(GL_TEXTURE_2D);
//    
//    glUseProgramObjectARB(Globals::edge_shader);
//    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 2);
//    glUniform1f(glGetUniformLocationARB(Globals::edge_shader, "width"), float(width));
//    glUniform1f(glGetUniformLocationARB(Globals::edge_shader, "height"), float(height));
//    //glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "RenderTex"), 0);
//    glutSolidTeapot(0.1);
//    object->pass = 2;
//    if(object){
//        object->OnDraw();
//    }
//    glUseProgramObjectARB(0);
//}
//
//void displayCallback()
//{
//    clock_t startTime = clock();
//    
////    // set rendering destination to FBO
////    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
////    
////    // clear buffers
////    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////    
////    // draw a scene to a texture directly
////    
////    
////    glViewport(0,0, width, height);
////    
////    glClearColor(1.,1.,1.,0.);
////    
////    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
////    
////    glMatrixMode(GL_PROJECTION);
////    glLoadIdentity();
////    gluPerspective(60.0, double(width)/(double)height, 0.1, 1000.0);
////    glMatrixMode(GL_MODELVIEW);
////    glLoadIdentity();
////    glTranslatef(0,0,-3);
////    glRotatef(angle, 0, 1, 0);
////    glutSolidTeapot(2);
////
////    
////    
////    glBindFramebuffer(GL_FRAMEBUFFER, 0);
////    
////    glViewport(0,0, width, height);
////    
////    glClearColor(1.,1.,1.,0.);
////    
////    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
////    
////    glMatrixMode(GL_PROJECTION);
////    glLoadIdentity();
////    gluPerspective(60.0, double(width)/(double)height, 0.1, 1000.0);
////    glMatrixMode(GL_MODELVIEW);
////    glLoadIdentity();
////    glTranslatef(0,0,-3);
////    glRotatef(angle, 0, 1, 0);
////    
////    //glActiveTexture(GL_TEXTURE0);
////    //glBindTexture(GL_TEXTURE_2D, texture);
////    
////    glEnable(GL_TEXTURE_2D);
////    glBindTexture(GL_TEXTURE_2D, rboId);
////    // Draw a textured quad
////    glBegin(GL_QUADS);
////    glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
////    glTexCoord2f(0, 1); glVertex3f(1, 2, 0);
////    glTexCoord2f(1, 1); glVertex3f(2, 2, 0);
////    glTexCoord2f(1, 0); glVertex3f(2, 1, 0);
////    glEnd();
////    
////    
////    glDisable(GL_TEXTURE_2D);
////    
////    glUseProgramObjectARB(Globals::edge_shader);
////    glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "pass"), 2);
////    glUniform1f(glGetUniformLocationARB(Globals::edge_shader, "width"), float(width));
////    glUniform1f(glGetUniformLocationARB(Globals::edge_shader, "height"), float(height));
////    //glUniform1i(glGetUniformLocationARB(Globals::edge_shader, "RenderTex"), 0);
////    glutSolidTeapot(0.1);
////    object->pass = 2;
////    if(object){
////        object->OnDraw();
////    }
////    glUseProgramObjectARB(0);
//
//    
//    
//    
//    
//    
//    
//    
//    
//    blur_prepare();
//    blur_final();
//    prepare();
//    final();
//
//    //angle = (angle + 1) % 360;
//    //Clear color and depth buffers
////    glBindFramebuffer(GL_FRAMEBUFFER, 0);
////    
////    glViewport(0,0, width, height);
////    
////    glClearColor(1.,1.,1.,0.);
////    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    //Set the OpenGL matrix mode to ModelView
//    glMatrixMode(GL_MODELVIEW);
//    //light->updateLight();
//    //Push a matrix save point
//    //This will save a copy of the current matrix so that we can
//    //make changes to it and 'pop' those changes off later.
//    //glUseProgramObjectARB(reflection_shader);
//    
//    
////    glPushMatrix();
////    glLoadIdentity();
////    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
////    glRotated(angle, 0, 1, 0);
//    
//    glBegin(GL_QUADS);
//    glNormal3f(0, 1, 0);
//    glColor3f(0.3, 0.3, 0.3);
//    glVertex3f( 100, -2, -100);
//    glVertex3f( 100, -2, 100);
//    glVertex3f( -100, -2, 100);
//    glVertex3f( -100, -2, -100);
//    glEnd();
//    glColor3f(1, 1, 1);
//    
//     int size_of_texture_cube = 100;
//     
//     
//     glUseProgramObjectARB(Globals::skybox_shader);
//     glBegin(GL_QUADS);
//     glNormal3f(0, 0, -1);
//     glVertex3f( size_of_texture_cube,-size_of_texture_cube,size_of_texture_cube );
//     glVertex3f(-size_of_texture_cube,-size_of_texture_cube,size_of_texture_cube);
//     glVertex3f(-size_of_texture_cube, size_of_texture_cube,size_of_texture_cube); //back up right
//     glVertex3f(size_of_texture_cube,size_of_texture_cube, size_of_texture_cube ); //back up left
//     glEnd();
//     
//     //Front[0]
//     
//     
//     glBegin(GL_QUADS);
//     glNormal3f(0, 0, 1);
//     glVertex3f(  -size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
//     glVertex3f( size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
//     glVertex3f( size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube);   //up right
//     glVertex3f(  -size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube); //up left
//     glEnd();
//     
//     
//     glBegin(GL_QUADS);
//     glNormal3f(-1, 0, 0);
//     glVertex3f(  -size_of_texture_cube, -size_of_texture_cube, size_of_texture_cube);
//     glVertex3f( -size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
//     glVertex3f( -size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube); //up
//     glVertex3f(  -size_of_texture_cube, size_of_texture_cube, size_of_texture_cube); //up
//     glEnd();
//     
//     
//     
//     glBegin(GL_QUADS);
//     glNormal3f(1, 0, 0);
//     glVertex3f( size_of_texture_cube, -size_of_texture_cube, -size_of_texture_cube);
//     glVertex3f( size_of_texture_cube, -size_of_texture_cube, size_of_texture_cube);
//     glVertex3f( size_of_texture_cube, size_of_texture_cube, size_of_texture_cube);
//     glVertex3f( size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube);
//     glEnd();
//     
//     glBegin(GL_QUADS);
//     glNormal3f(0.0, 1.0, 0.0);
//     glVertex3f( -size_of_texture_cube, size_of_texture_cube, size_of_texture_cube); //connect to back up left
//     glVertex3f( -size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube); //connect to front up left
//     
//     glVertex3f( size_of_texture_cube, size_of_texture_cube, -size_of_texture_cube);
//     glVertex3f( size_of_texture_cube, size_of_texture_cube, size_of_texture_cube);
//     glEnd();
//    
//     glTranslatef(2, 0, 0);
//     object->pass = 3;
//     if(object){
//         object->OnDraw();
//     }
//     glTranslatef(-4, 0, 0);
//     object->pass = 4;
//     if(object){
//        object->OnDraw();
//     }
//     glTranslatef(2, 1, 0);
//     object->pass = 5;
//     if(object){
//        object->OnDraw();
//     }
//     glTranslatef(0, -2, 0);
//     object->pass = 6;
//     if(object){
//        object->OnDraw();
//     }
//    glUseProgramObjectARB(0);
//
//    
//    
//    //glutSolidSphere(1, 20, 20);
//    //glUseProgramObjectARB(0);
//    //glutSolidTeapot(1);
//    clock_t endTime = clock();
//    cout << "frame rate: " << 1.0 / (float((endTime - startTime)) / CLOCKS_PER_SEC) << endl;
//    glPopMatrix();
//    glFlush();
//    //Swap the off-screen buffer (the one we just drew to) with the on-screen buffer
//    glutSwapBuffers();
//    
//}
//
////----------------------------------------------------------------------------
//// Callback method called when system is idle.
//// This is called at the start of every new "frame" (qualitatively)
//void idleCallback()
//{
//    //Call the display routine to draw the cube
//    displayCallback();
//}
//
////----------------------------------------------------------------------------
//// Callback method called by GLUT when graphics window is resized by the user
//void reshapeCallback(int w, int h)
//{
//    width = w;                                                       //Set the window width
//    height = h;                                                      //Set the window height
//    glViewport(0, 0, w, h);                                          //Set new viewport size
//    glMatrixMode(GL_PROJECTION);                                     //Set the OpenGL matrix mode to Projection
//    glLoadIdentity();                                                //Clear the projection matrix by loading the identity
//    gluPerspective(60.0, double(width)/(double)height, 0.0, 1000.0); //Set perspective projection viewing frustum
//    //glTranslatef(0, 0, -10);
//    glMatrixMode(GL_MODELVIEW);
//    setupFBO();
//    //setupShadowFBO();
//    
//}
//void processNormalKeys(unsigned char key, int x, int y){
//    if (key == 27){
//        exit(0);
//    }
//    if(key == 'r'){
//        angle = (angle + 1) % 360;
//    }
//    else if(key == 'v'){
//        angle_z = (angle_z + 1) % 360;
//    }
//}
//
//
//
//
//int main(int argc, char *argv[])
//{
//    //GLUT and OpenGL Configuration
//    glutInit(&argc, argv);                                      //Initialize GLUT
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   //Open an OpenGL context with double buffering, RGB colors, and depth buffering
//    
//    glutInitWindowSize(512, 512);          //Set initial window size
//    glutCreateWindow("Graphics Engine"); //Open window and set window title
//    glEnable(GL_DEPTH_TEST);                        //Enable depth buffering
//    glClear(GL_DEPTH_BUFFER_BIT);                               //Clear depth buffer
//    glClearColor(0.0, 0.0, 0.0, 0.0);                           //Set clear color to black
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                  //Set polygon drawing mode to fill front and back of each polygon
//    glDisable(GL_CULL_FACE);                                    //Disable backface culling to render both sides of polygons
//    glShadeModel(GL_SMOOTH);                                    //Set shading to smooth
//    
//    glEnable(GL_COLOR_MATERIAL);                                //Enable color materials
//    //glEnable(GL_LIGHTING);                                      //Enable lighting
//    glEnable(GL_NORMALIZE);                                     //Auto normalize surface normals
//    
//    //Register callback functions:
//    glutDisplayFunc(displayCallback);
//    //glutDisplayFunc(display);
//    glutReshapeFunc(reshapeCallback);
//    glutIdleFunc(idleCallback);
//    //glutIdleFunc(glutPostRedisplay);
//    glutKeyboardFunc(processNormalKeys);
//    
//    const GLubyte *renderer = glGetString(GL_RENDERER);
//    const GLubyte *vendor = glGetString(GL_VENDOR);
//    const GLubyte *version = glGetString(GL_VERSION);
//    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
//    
//    
//    printf("vendor %s\n", vendor);
//    printf("renderer %s\n", renderer);
//    printf("version %s\n", version);
//    printf("glsl %s\n", glslVersion);
//    
//    //Basic light with normal shader
//    GLhandleARB vertexShaderHandle;
//    GLhandleARB fragmentShaderHandle;
//    //vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/multiple_light.vert", GL_VERTEX_SHADER);
//    //fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/multiple_light.frag", GL_FRAGMENT_SHADER);
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag", GL_FRAGMENT_SHADER);
//    Globals::light_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::light_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::light_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::light_shader);
//    
//    
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/multiple_light.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/multiple_light.frag", GL_FRAGMENT_SHADER);
//    Globals::multiply_light_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::multiply_light_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::multiply_light_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::multiply_light_shader);
//    
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/edge.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/edge.frag", GL_FRAGMENT_SHADER);
//    Globals::edge_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::edge_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::edge_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::edge_shader);
//    
//    
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/blur.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/blur.frag", GL_FRAGMENT_SHADER);
//    Globals::blur_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::blur_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::blur_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::blur_shader);
//    
//    
//    //Reflection shader
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/reflect.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/reflect.frag", GL_FRAGMENT_SHADER);
//    Globals::reflection_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::reflection_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::reflection_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::reflection_shader);
//    
//    
//    //Refraction shader
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/refract.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/refract.frag", GL_FRAGMENT_SHADER);
//    Globals::refraction_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::refraction_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::refraction_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::refraction_shader);
//    
//    //Skybox shader
//    vertexShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/skybox.vert", GL_VERTEX_SHADER);
//    fragmentShaderHandle = loadShader("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/skybox.frag", GL_FRAGMENT_SHADER);
//    Globals::skybox_shader = glCreateProgramObjectARB();
//    glAttachObjectARB(Globals::skybox_shader, vertexShaderHandle);
//    glAttachObjectARB(Globals::skybox_shader, fragmentShaderHandle);
//    glLinkProgramARB(Globals::skybox_shader);
//    
//    
//    faces.clear();
//    getSkybox(CLOUDS);
//    GLuint cubemapTexture = loadCubemap(faces);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
//    
//    //glUniform1i(glGetUniformLocationARB(shader_id, "tex"), 0);
//    std::cout << glGetError() << std::endl;
//    //Start up the render loop!
//    
//    initialize();
//    glutMainLoop();
//    
//    return 0;
//}
//
//void getSkybox(int type){
//    switch(type){
//        case ALIEN:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/alien/jajalien1_front.jpg");
//            break;
//            
//        case BLUE_SKY:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue sky/bluesky_front.jpg");
//            break;
//        case BLUE_GREEN:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/blue_green/nansen_front.jpg");
//            break;
//        case CALM:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/calm/calm_front.jpg");
//            break;
//        case CLOUDS:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/clouds/clouds_front.jpg");
//            break;
//        case EMERALD:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emerald/emerald_front.jpg");
//            break;
//        case EMERALD_FOG:
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_left.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_right.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_top.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_back.jpg");
//            faces.push_back("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/Skyboxes/emeraldfog/emeraldfog_front.jpg");
//            break;
//    }
//}
//
