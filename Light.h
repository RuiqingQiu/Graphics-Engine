//
//  Light.h
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/21/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#ifndef __Graphics_Engine__Light__
#define __Graphics_Engine__Light__
#include <GLUT/glut.h>
#include <stdio.h>
class Light{
public:
    float x, y, z;
    Light();
    ~Light();
    void updateLight();
    void setLightPosition(float a, float b, float c);
};
#endif /* defined(__Graphics_Engine__Light__) */
