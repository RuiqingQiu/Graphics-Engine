attribute vec4 VertexTangent;

varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec3 Tangent;

varying vec3 LightDir;
varying vec3 ViewDir;

uniform mat4 ModelView;

void main()
{
    Normal = normalize( gl_NormalMatrix * gl_Normal);
    Position = gl_ModelViewMatrix * vec4(gl_Vertex.xyz,1.0);
    
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
}