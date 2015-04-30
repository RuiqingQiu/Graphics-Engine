//
//  ShaderSystem.cpp
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/30/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//
#include "ShaderSystem.h"

GLhandleARB loadShaderFromFile(char* filename, unsigned int type)
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

ShaderSystem::ShaderSystem()
{
    loadShaders("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert",
                "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag");
    /*
    loadShaders("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/skybox.vert",
                "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/skybox.frag");
    loadShaders("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/reflect.vert",
                "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/reflect.frag");
    loadShaders("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/refract.vert", "/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/refract.frag");
     */
    
}


ShaderSystem::~ShaderSystem()
{
}

/*
 * This function is for loading a shader into the prebuilt shader list
 */
bool ShaderSystem::loadShaders(string vertex_shader_path, string fragment_shader_path){
    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;
    
    char *v_str = new char[vertex_shader_path.length() + 1];
    strcpy(v_str, vertex_shader_path.c_str());
    char *f_str = new char[fragment_shader_path.length() + 1];
    strcpy(f_str, fragment_shader_path.c_str());
    
    vertexShaderHandle = loadShaderFromFile("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.vert", GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShaderFromFile("/Users/ruiqingqiu/Desktop/Qiu_Code/Graphics Engine/light.frag", GL_FRAGMENT_SHADER);
    
    //vertexShaderHandle = loadShaderFromFile(v_str, GL_VERTEX_SHADER);
    //fragmentShaderHandle = loadShaderFromFile(f_str, GL_FRAGMENT_SHADER);
    
    GLhandleARB shader_id = glCreateProgramObjectARB();
    shader_ids.push_back(shader_id);
    glAttachObjectARB(shader_id, vertexShaderHandle);
    glAttachObjectARB(shader_id, fragmentShaderHandle);
    glLinkProgramARB(shader_id);
    return true;
}

void ShaderSystem::BindShader(int type){
    switch (type){
        case REGULAR_SHADER:
            glUseProgramObjectARB(shader_ids[type]);
            break;
        case SKYBOX_SHADER:
            glUseProgramObjectARB(shader_ids[type]);
            break;
        case REFLECTION_SHADER:
            glUseProgramObjectARB(shader_ids[type]);
            break;
        case REFRACTION_SHADER:
            glUseProgramObjectARB(shader_ids[type]);
            break;
    }
    
}
void ShaderSystem::UnbindShader(){
    glUseProgramObjectARB(0);
}