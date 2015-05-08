// Used for shadow lookup
varying vec4 ShadowCoord;

varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;


void main()
{
    Normal = normalize( gl_NormalMatrix * gl_Normal);
    Position = gl_ModelViewMatrix * vec4(gl_Vertex.xyz,1.0);
    TexCoords= vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
    
    
    ShadowCoord= gl_TextureMatrix[7] * gl_Vertex;
    
    gl_Position = ftransform();
    
    gl_FrontColor = gl_Color;
}
