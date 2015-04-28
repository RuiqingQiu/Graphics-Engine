//#version 410
varying vec3 vN;
varying vec3 v;

attribute vec3 tangent;
attribute vec3 binormal;

uniform vec3 CAMERA_POSITION;
varying vec3 eyeVec;
varying vec3 position;
varying vec3 lightvec;

//Mode = 0 for no shading
//Mode = 1 for texture only
//Mode = 2 for normal and texture
varying vec3 mode;

uniform float light_x;
uniform float light_y;
uniform float light_z;
void main(void)
{
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
    vN = normalize(gl_NormalMatrix * gl_Normal);
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = ftransform();
    position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    //position = gl_Vertex.xyz;
    //Compute TBN Matrix
    mat3 TBNMatrix = mat3(tangent, binormal, gl_Normal);
    
    //Compute eye vector
    eyeVec = CAMERA_POSITION - position;
    eyeVec *= TBNMatrix;
    
    //lightvec = gl_LightSource[0].position.xyz;
    //Compute light vector
    lightvec = vec3(light_x, light_y, light_z) - position;
    //lightvec = gl_LightSource[0].position.xyz - position;
    //vec3 light_pos = vec3(light_x, light_y, light_z);
    //light_pos = (gl_ModelViewMatrix * vec4(light_pos, 1)).xyz;
    //lightvec = light_pos - position;
    //lightvec = normalize(lightvec);
    lightvec *= TBNMatrix;
    //lightvec = vec3(light_x, light_y, light_z);
    mode = vec3(2,0,0);
    //lightvec = vec3(light_x, light_y, light_z);
}



/* Working with texture normal and gloss
 varying vec3 vN;
 varying vec3 v;
 attribute vec3 tangent;
 attribute vec3 binormal;
 uniform vec3 CAMERA_POSITION;
 varying vec3 eyeVec;
 varying vec3 position;
 varying vec3 lightvec;
 //Mode = 0 for no shading
 //Mode = 1 for texture only
 //Mode = 2 for normal and texture
 varying vec3 mode;
 void main(void)
 {
 v = vec3(gl_ModelViewMatrix * gl_Vertex);
 vN = normalize(gl_NormalMatrix * gl_Normal);
 gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
 gl_Position = ftransform();
 position = gl_Vertex.xyz;
 
 //Compute TBN Matrix
 mat3 TBNMatrix = mat3(tangent, binormal, gl_Normal);
 
 //Compute eye vector
 eyeVec = CAMERA_POSITION - position;
 eyeVec *= TBNMatrix;
 //Compute light vector
 lightvec = gl_LightSource[0].position.xyz - position;
 lightvec *= TBNMatrix;
 mode = vec3(2,0,0);
 }
 */



/* working version for normal and texture */
/*
 varying vec3 vN;
 varying vec3 v;
 attribute vec3 tangent;
 attribute vec3 binormal;
 varying vec3 position;
 varying vec3 lightvec;
 //Mode = 0 for no shading
 //Mode = 1 for texture only
 //Mode = 2 for normal and texture
 varying vec3 mode;
 void main(void)
 {
 v = vec3(gl_ModelViewMatrix * gl_Vertex);
 vN = normalize(gl_NormalMatrix * gl_Normal);
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
 gl_Position = ftransform();
 position = gl_Vertex.xyz;
 mat3 TBNMatrix = mat3(tangent, binormal, gl_Normal);
 lightvec = gl_LightSource[0].position.xyz - position;
 lightvec *= TBNMatrix;
 mode = vec3(2,0,0);
 }
 */



















/* Working texture shader */
/*
 varying vec3 vN;
 varying vec3 v;
 void main(void)
 {
 v = vec3(gl_ModelViewMatrix * gl_Vertex);
 vN = normalize(gl_NormalMatrix * gl_Normal);
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 gl_TexCoord[0] = gl_MultiTexCoord0;
 gl_Position = ftransform();
 }
 */