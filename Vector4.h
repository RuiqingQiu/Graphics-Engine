//
//  Vector4.h
//  CSE167HW1
//
//  Created by Raymond Qiu on 10/8/14.
//  Copyright (c) 2014 Ruiqing Qiu. All rights reserved.
//

#ifndef __CSE167HW1__Vector4__
#define __CSE167HW1__Vector4__

#include <iostream>
#include <math.h>
using namespace std;
class Vector4
{
    
public:
    double x, y, z, w;
    Vector4(double x, double y, double z, double w);// constructor with four parameters for the vector coordinates
    Vector4 operator+(const Vector4&);// overload operator '+' for addition
    Vector4 operator-(const Vector4&);// overload operator '-' for subtraction
    void dehomogenize();// dehomogenize the vector (scale it so that its fourth component is equal to one)
    void print(string comment);// print x,y, z and w components of the vector after a comment string
    double get_x();
    double get_y();
    double get_z();
    double get_w();
    friend class Matrix4;
    double length();
};
#endif /* defined(__CSE167HW1__Vector4__) */
