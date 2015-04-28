varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec3 Tangent;

varying vec3 LightDir;
varying vec3 ViewDir;

uniform vec4 VertexTangent;

void main()
{
    TexCoords= vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
    Normal = normalize( gl_NormalMatrix * gl_Normal);
    Position = gl_ModelViewMatrix * gl_Vertex;
  
    Tangent = normalize(gl_NormalMatrix * vec3(VertexTangent));
    vec3 binormal = normalize(cross(Normal, Tangent)) * VertexTangent.w;
    mat3 toObjectLocal = mat3(
    Tangent.x, binormal.x, Normal.x,
    Tangent.y, binormal.y, Normal.y,
    Tangent.z, binormal.z, Normal.z);
    vec4 LightPosition = vec4(0.0, 10.0, 20.0, 1.0);
    LightDir = normalize(toObjectLocal * (LightPosition.xyz - Position.xyz));
    ViewDir = toObjectLocal * normalize(-Position.xyz);
  
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}