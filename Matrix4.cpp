#include <math.h>

#include "Matrix4.h"

Matrix4::Matrix4()
{
  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<4; ++j)
    {
      m[i][j] = 0;
    }
  }
}

Matrix4::Matrix4(double n1){
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3][3] = 1;
}

Matrix4::Matrix4(Vector3 x, Vector3 y, Vector3 z, Vector3 e){
    m[0][0] = x.x;
    m[1][0] = x.y;
    m[2][0] = x.z;
    m[3][0] = 0;
 
    m[0][1] = y.x;
    m[1][1] = y.y;
    m[2][1] = y.z;
    m[3][1] = 0;
    
    m[0][2] = z.x;
    m[1][2] = z.y;
    m[2][2] = z.z;
    m[3][2] = 0;
    
    m[0][3] = e.x;
    m[1][3] = e.y;
    m[2][3] = e.z;
    m[3][3] = 1;
}


Matrix4& Matrix4::operator=(const Matrix4& m2)
{
  if (this != &m2)
  {
    for (int i=0; i<4; ++i)
    {
      for (int j=0; j<4; ++j)
      {
        m[i][j] = m2.m[i][j];
      }
    }
  }
  return *this;
}

// return pointer to matrix elements
double* Matrix4::getPointer()
{
  return &m[0][0];
}

// set matrix to identity matrix
void Matrix4::identity()
{
  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<4; ++j)
    {
      if (i==j) 
        m[i][j] = 1.0; 
      else 
        m[i][j] = 0.0;
    }
  }
}

// transpose the matrix (mirror at diagonal)
void Matrix4::transpose()
{
  Matrix4 temp;
  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<4; ++j)
    {
      temp.m[j][i] = m[i][j];
    }
  }
  *this = temp;  // copy temporary values to this matrix
}

// Creates a rotation matrix which rotates about the y axis.
// angle is expected in degrees
void Matrix4::makeRotateY(double angle)
{
    angle = angle / 180.0 * M_PI;  // convert from degrees to radians
    identity();
	m[0][0] = cos(angle);
	m[0][2] = sin(angle);
	m[2][0] = -sin(angle);
	m[2][2] = cos(angle);
}

void Matrix4::makeRotateX(double angle){
    angle = angle / 180.0 * M_PI;//Convert from degrees to radians
    identity();
    m[1][1] = cos(angle);
    m[1][2] = -sin(angle);
    m[2][1] = sin(angle);
    m[2][2] = cos(angle);
    
}// make rotation matrix about X axis with angle in degrees (note that the sin/cos functions in C++ expect radians so you need to convert to radians within the fmakeRotateZ(double angle);// make rotation matrix about Z axis with angle in degrees
void Matrix4::makeRotateZ(double angle){
    angle = angle / 180.0 * M_PI;//Convert from degrees to radians
    identity();
    m[0][0] = cos(angle);
    m[0][1] = -sin(angle);
    m[1][0] = sin(angle);
    m[1][1] = cos(angle);
}

void Matrix4::makeRotate(double angle, const Vector3& axis){
    angle = angle / 180.0 * M_PI;//Convert from degrees to radians
    identity();
    float sinAngle, cosAngle;
    float mag = sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    
    sinAngle = sinf (angle);
    cosAngle = cosf (angle);
    float xx, yy, zz, xy, yz, zx, xs, ys, zs;
    float oneMinusCos;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    x /= mag;
    y /= mag;
    z /= mag;
    
    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * sinAngle;
    ys = y * sinAngle;
    zs = z * sinAngle;
    oneMinusCos = 1.0f - cosAngle;
    
    m[0][0] = (oneMinusCos * xx) + cosAngle;
    m[0][1] = (oneMinusCos * xy) - zs;
    m[0][2] = (oneMinusCos * zx) + ys;
    m[0][3] = 0.0F;
    
    m[1][0] = (oneMinusCos * xy) + zs;
    m[1][1] = (oneMinusCos * yy) + cosAngle;
    m[1][2] = (oneMinusCos * yz) - xs;
    m[1][3] = 0.0F;
    
    m[2][0] = (oneMinusCos * zx) - ys;
    m[2][1] = (oneMinusCos * yz) + xs;
    m[2][2] = (oneMinusCos * zz) + cosAngle;
    m[2][3] = 0.0F;
    
    m[3][0] = 0.0F;
    m[3][1] = 0.0F;
    m[3][2] = 0.0F;
    m[3][3] = 1.0F;
}// Make a rotation matrix about an arbitrary axis


Matrix4 Matrix4::operator*(const Matrix4& m2){
    Matrix4* product = new Matrix4();
    for(int row = 0; row < 4; row++){
        for(int col = 0; col < 4; col++){
            for(int inner = 0; inner < 4; inner++){
                product->m[row][col] += this->m[row][inner]*m2.m[inner][col];
            }
        }
    }
    return *product;
}//multiply matrix with matrix

Vector4 Matrix4::operator*(const Vector4& v){
    double x = 0.0, y = 0.0, z = 0.0, w = 0.0;
    x += this->m[0][0]*v.x;
    x += this->m[0][1]*v.y;
    x += this->m[0][2]*v.z;
    x += this->m[0][3]*v.w;
    
    y += this->m[1][0]*v.x;
    y += this->m[1][1]*v.y;
    y += this->m[1][2]*v.z;
    y += this->m[1][3]*v.w;
    
    
    z += this->m[2][0]*v.x;
    z += this->m[2][1]*v.y;
    z += this->m[2][2]*v.z;
    z += this->m[2][3]*v.w;
    
    w += this->m[3][0]*v.x;
    w += this->m[3][1]*v.y;
    w += this->m[3][2]*v.z;
    w += this->m[3][3]*v.w;
    
    return Vector4(x,y,z,w);
}//multiply matrix with vector



void Matrix4::makeScale(double sx, double sy, double sz){
    identity();
    this->m[0][0] = this->m[0][0] * sx;
    this->m[0][1] = this->m[0][1] * sx;
    this->m[0][2] = this->m[0][2] * sx;
    
    this->m[1][0] = this->m[1][0] * sy;
    this->m[1][1] = this->m[1][1] * sy;
    this->m[1][2] = this->m[1][2] * sy;
    
    this->m[2][0] = this->m[2][0] * sz;
    this->m[2][1] = this->m[2][1] * sz;
    this->m[2][2] = this->m[2][2] * sz;
}// make a non-uniform scaling matrix

void Matrix4::makeTranslate(double tx, double ty, double tz){
    identity();
    this->m[0][3] = tx;
    this->m[1][3] = ty;
    this->m[2][3] = tz;
}// make a translation matrix
void Matrix4::print(string comment){
    cout << comment << endl;
    for(int row = 0; row < 4; row++){
        cout << this->m[row][0] << ", " << this->m[row][1] << ", " << this->m[row][2] << ", " << this->m[row][3] << endl;
    }
}// print the matrix (display all 16 matrix components numerically on the screen in a 4x4 array)

void Matrix4::resetTranslate(){
    getPointer()[12] = 0;
    getPointer()[13] = 0;
    getPointer()[14] = 0;
}

float invf(int i,int j,const float* m){
    
    int o = 2+(j-i);
    
    i += 4+o;
    j += 4-o;
    
#define e(a,b) m[ ((j+b)%4)*4 + ((i+a)%4) ]
    
    float inv =
    + e(+1,-1)*e(+0,+0)*e(-1,+1)
    + e(+1,+1)*e(+0,-1)*e(-1,+0)
    + e(-1,-1)*e(+1,+0)*e(+0,+1)
    - e(-1,-1)*e(+0,+0)*e(+1,+1)
    - e(-1,+1)*e(+0,-1)*e(+1,+0)
    - e(+1,-1)*e(-1,+0)*e(+0,+1);
    
    return (o%2)?inv : -inv;
    
#undef e
    
}

Matrix4 Matrix4::inverseMatrix4x4(const float *m)
{
    
    float inv[16];
    
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            inv[j*4+i] = invf(i,j,m);
    
    double D = 0;
    
    for(int k=0;k<4;k++) D += m[k] * inv[k*4];
    
    if (D == 0) return false;
    
    D = 1.0 / D;
    float out[16];
    for (int i = 0; i < 16; i++)
        out[i] = inv[i] * D;
    Matrix4 tmp = Matrix4();
    tmp.m[0][0] = out[0];
    tmp.m[0][1] = out[1];
    tmp.m[0][2] = out[2];
    tmp.m[0][3] = out[3];
    
    tmp.m[1][0] = out[4];
    tmp.m[1][1] = out[5];
    tmp.m[1][2] = out[6];
    tmp.m[1][3] = out[7];
    
    tmp.m[2][0] = out[8];
    tmp.m[2][1] = out[9];
    tmp.m[2][2] = out[10];
    tmp.m[2][3] = out[11];
    
    tmp.m[3][0] = out[12];
    tmp.m[3][1] = out[13];
    tmp.m[3][2] = out[14];
    tmp.m[3][3] = out[15];

    return tmp;
    
}

