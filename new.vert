#version 410
layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;

out vec3 Color;

uniform mat4 RotationMatrix;
uniform vec3 p_location;
void main(){
    Color = VertexColor;
    Color = p_location;
    gl_Position = RotationMatrix * vec4(VertexPosition, 1.0);
    
    //gl_Position = vec4(VertexPosition, 1.0);
}