//
//  Light.cpp
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/21/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#include "Light.h"
float x, y, z;
Light::Light(){
    x = 0;
    y = 0;
    z = 0;
}
Light::~Light(){
    
}

void Light::setLightPosition(float a, float b, float c){
    x = a;
    y = b;
    z = c;
}

void Light::updateLight(){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(1, 0, 0);
    glutSolidSphere(1, 20, 20);
    float LightPosition[] = {x, y, z, 1};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glPopMatrix();
    glColor3f(1,1,1);
    
}