varying vec3 Normal;
varying vec3 Position;
varying vec4 ShadowCoord;

uniform mat4 ShadowMatrix;

void main()
{
    Position = (gl_ModelViewMatrix * vec4(gl_Vertex.xyz,1.0)).xyz;
    Normal = normalize( gl_NormalMatrix * gl_Normal );
    ShadowCoord = ShadowMatrix * vec4(gl_Vertex.xyz,1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
}