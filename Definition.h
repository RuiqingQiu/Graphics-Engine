//
//  Definition.h
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/30/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//

#ifndef Graphics_Engine_Definition_h
#define Graphics_Engine_Definition_h

enum ModelType{
    //blocks
    BasicCube = 0,
    GlowingCube = 1,
    WoodenCube = 2,
    
    //weapons
    Mace = 3,
    Mallet = 4,
    Needle = 5,
    
    //wheels
    Discount = 6,
    Tire = 7,
    WoodenWheel = 8,
    
    //Other
    BATTLEFIELD = 9,
    
    THREEBYTHREE_BASIC = 10,
    THREEBYTHREE_GLOWING = 11,
    THREEBYTHREE_WOODEN = 12,
    THREEBYTHREE_WHEEL_DISCOUNT = 13,
    THREEBYTHREE_WHEEL_TIRE = 14,
    THREEBYTHREE_WHEEL_WOODEN = 15,
    
    TEMPLATE_1 = 21,
    TEMPLATE_2 = 22,
    TEMPLATE_3 = 23,
    
    WALL = 101,
    BULLET_1 = 102,
    
};

enum ShaderType{
    REGULAR_SHADER = 0,
    SKYBOX_SHADER = 1,
    REFLECTION_SHADER = 2,
    REFRACTION_SHADER = 3,
};

enum SkyboxType{
    ALIEN = 0,
    BLUE_SKY = 1,
    BLUE_GREEN = 2,
    CALM = 3,
    CLOUDS = 4,
    EMERALD = 5,
    EMERALD_FOG = 6
    
};

#endif
