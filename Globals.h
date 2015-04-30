//
//  Globals.h
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/30/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#ifndef __Graphics_Engine__Globals__
#define __Graphics_Engine__Globals__

#include <stdio.h>
#include "ShaderSystem.h"

class Globals{
public:
    //static ShaderSystem* shader_system;
    static GLhandleARB light_shader;
    static GLhandleARB refraction_shader;
    static GLhandleARB reflection_shader;
    static GLhandleARB skybox_shader;
};
#endif /* defined(__Graphics_Engine__Globals__) */
