varying vec4 Position;
varying vec3 Normal;

void main()
{
    Normal = normalize( gl_NormalMatrix * gl_Normal);
    Position = gl_ModelViewMatrix * vec4(gl_Vertex.xyz,1.0);
    
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
}