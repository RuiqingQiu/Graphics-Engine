//
//  Vector3.h
//  CSE167HW1
//
//  Created by Raymond Qiu on 10/8/14.
//  Copyright (c) 2014 Ruiqing Qiu. All rights reserved.
//

#ifndef __CSE167HW1__Vector3__
#define __CSE167HW1__Vector3__

#include <iostream>
using namespace std;
class Vector3
{
    
public:
    double x,y,z;
    Vector3();
    Vector3(double x, double y, double z);//constructor with three parameters for the vector coordinates
    Vector3 operator+(const Vector3&); //overload operator '+' for addition
    Vector3 operator-(const Vector3&); //overload operator '-' for subtraction
    void negate(); //negation
    void scale(double s);// scale (multiplication with scalar value)
    double dot(const Vector3&, const Vector3&);//dot product, returns result
    Vector3 cross(const Vector3&, const Vector3&);//cross product, returns result and puts it in calling vector
    double length(); // length of the vector
    void normalize(); //normalize the vector (make it so that its length is equal to one)
    void print(string comment);// print x,y and z components of the vector after a comment string
    friend class Matrix4;
    double distance(const Vector3&v1);
    
    double getX();
    double getY();
    double getZ();
    void setX(double mx);
    void setY(double my);
    void setZ(double mz);
    
};
#endif /* defined(__CSE167HW1__Vector3__) */
