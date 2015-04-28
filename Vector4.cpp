//
//  Vector4.cpp
//  CSE167HW1
//
//  Created by Raymond Qiu on 10/8/14.
//  Copyright (c) 2014 Ruiqing Qiu. All rights reserved.
//

#include "Vector4.h"
Vector4::Vector4(double x, double y, double z, double w){
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}// constructor with four parameters for the vector coordinates
Vector4 Vector4::operator+(const Vector4& v){
    double x = this->x + v.x;
    double y = this->y + v.y;
    double z = this->z + v.z;
    double w = this->w + v.w;
    return *new Vector4(x,y,z,w);
}// overload operator '+' for addition
Vector4 Vector4::operator-(const Vector4& v){
    double x = this->x - v.x;
    double y = this->y - v.y;
    double z = this->z - v.z;
    double w = this->w - v.w;
    return *new Vector4(x,y,z,w);
}// overload operator '-' for subtraction
void Vector4::dehomogenize(){
    double factor = this->w;
    this->x = this->x / factor;
    this->y = this->y / factor;
    this->z = this->z / factor;
    this->w = this->w / factor;
}// dehomogenize the vector (scale it so that its fourth component is equal to one)
void Vector4::print(string comment){
     cout << comment << this->x << ", " << this->y << ", " << this->z << ", " << this->w << endl;
}
double Vector4::length(){
    return sqrt(this->x*this->x + this->y*this->y +this->z*this->z);
}// length of the vector

double Vector4::get_x(){
    return this->x;
}
double Vector4::get_y(){
    return this->y;
}
double Vector4::get_z(){
    return this->z;
}
double Vector4::get_w(){
    return this->w;
}
