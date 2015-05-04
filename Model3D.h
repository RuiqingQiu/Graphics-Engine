//
//  Model3D.h
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/21/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#ifndef __Graphics_Engine__Model3D__
#define __Graphics_Engine__Model3D__

#include <stdio.h>
#include <string>
#include <iostream>
#include "tiny_obj_loader.h"
#include <GLUT/glut.h>
#include "Matrix4.h"
using namespace std;

class Model3D
{
public:
    
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    int shader_type;
    int pass;
    GLuint texturaID[4];
    
    string texture_map;
    string normal_map;
    string gloss_map;
    string metallic_map;
    Model3D();
    Model3D(string filename, string texture, string normal, string gloss, string metallic);
    
    ~Model3D();
    void setTextureMap(string pathname);
    void setNormalMap(string pathname);
    void setGlossMap(string pathname);
    void setMetallicMap(string pathname);

    void OnDraw();
private:
    bool isTextured = false;
};
#endif /* defined(__Graphics_Engine__Model3D__) */
