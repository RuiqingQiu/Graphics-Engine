//
//  ShaderSystem.h
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/30/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#ifndef __Graphics_Engine__ShaderSystem__
#define __Graphics_Engine__ShaderSystem__

#include <stdio.h>
#include "tiny_obj_loader.h"
#include "GLUT/glut.h"
#include <string>
#include "Definition.h"
#include <iostream>
#include "SOIL.h"

class ShaderSystem
{
public:
    //Shader id
    vector<GLhandleARB> shader_ids;
    //Four maps
    GLuint texturaID[4];
    
    
    ShaderSystem();
    bool loadShaders(string vertex_shader_path, string fragment_shader_path);
    void BindShader(int type);
    void UnbindShader();
    ~ShaderSystem();
};
#endif /* defined(__Graphics_Engine__ShaderSystem__) */
