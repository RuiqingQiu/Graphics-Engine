//
//  Vector3.cpp
//  CSE167HW1
//
//  Created by Raymond Qiu on 10/8/14.
//  Copyright (c) 2014 Ruiqing Qiu. All rights reserved.
//

#include "Vector3.h"
#include <math.h>
#include <iostream>
using namespace std;

double Vector3::getX()
{
    return x;
}

double Vector3::getY()
{
    return y;
}

double Vector3::getZ()
{
    return z;
}

void Vector3::setX(double mx)
{
    x = mx;
}

void Vector3::setY(double my)
{
    y = my;
}

void Vector3::setZ(double mz)
{
    z = mz;
}

Vector3::Vector3()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}
Vector3::Vector3(double x, double y, double z){
    //constructor with three parameters for the vector coordinates
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3 Vector3::operator+(const Vector3& v){
    //overload operator '+' for addition
    double x = this->x + v.x;
    double y = this->y + v.y;
    double z = this->z + v.z;
    return Vector3(x,y,z);
}
Vector3 Vector3::operator-(const Vector3& v){
    double x = this->x - v.x;
    double y = this->y - v.y;
    double z = this->z - v.z;
    return Vector3(x,y,z);
}//overload operator '-' for subtraction

void Vector3::negate(){
    this->x = -this->x;
    this->y = -this->y;
    this->z = -this->z;
}//negation

void Vector3::scale(double s){
    this->x = s * this->x;
    this->y = s * this->y;
    this->z = s * this->z;
}// scale (multiplication with scalar value)

double Vector3::dot(const Vector3& v1, const Vector3& v2){
    return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}//dot product, returns result

double Vector3::distance(const Vector3&v1){
    return sqrt((this->x - v1.x)*(this->x - v1.x) + (this->y - v1.y)*(this->y - v1.y) + (this->z - v1.z)* (this->z - v1.z));
}

Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2){
    double x = v1.y*v2.z - v1.z*v2.y;
    double y = v1.z*v2.x - v1.x*v2.z;
    double z = v1.x*v2.y - v1.y*v2.x;
    return Vector3(x, y, z);
}//cross product, returns result and puts it in calling vector

double Vector3::length(){
    return sqrt(this->x*this->x + this->y*this->y +this->z*this->z);
}// length of the vector

void Vector3::normalize() {
    double length = this->length();
    this->x = this->x / length;
    this->y = this->y / length;
    this->z = this->z / length;
}//normalize the vector (make it so that its length is equal to one)

void Vector3::print(string comment){
    cout << comment << this->x << ", " << this->y << ", " << this->z << endl;
}// print x,y and z components of the vector after a comment string
