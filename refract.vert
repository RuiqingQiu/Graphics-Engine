attribute vec4 VertexTangent;
varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;
varying vec3 Tangent;
varying vec3 LightDir;
varying vec3 ViewDir;


varying vec3 ReflectDir;
varying vec3 RefractDir;
vec3 WorldCameraPosition;
uniform mat4 ModelView;

void main()
{
    TexCoords= vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
    Normal = normalize( gl_NormalMatrix * gl_Normal);
    Position = gl_ModelViewMatrix * gl_Vertex;

    
    //Normal Map
    Tangent = normalize(gl_NormalMatrix * vec3(VertexTangent));
    vec3 binormal = normalize(cross(Normal, Tangent)) * VertexTangent.w;
    mat3 toObjectLocal = mat3(
                              Tangent.x, binormal.x, Normal.x,
                              Tangent.y, binormal.y, Normal.y,
                              Tangent.z, binormal.z, Normal.z);
    vec4 light_pos = vec4(0.0, 10.0, 20.0, 1.0);
    LightDir = normalize(toObjectLocal * (light_pos.xyz - Position.xyz));
    ViewDir = toObjectLocal * normalize(-Position.xyz);
    
    //Reflect and Refract
    mat4 tmp = mat4(1.0);
    WorldCameraPosition = vec3(0, 0, 0);
    vec3 worldPos = vec3(ModelView * vec4(gl_Vertex.xyz,1.0));
    vec3 worldNorm = vec3(ModelView * vec4(gl_Normal.xyz,0.0));
    //vec3 worldPos = vec3(tmp * vec4(gl_Vertex.xyz,1.0));
    //vec3 worldNorm = vec3(tmp * vec4(gl_Normal.xyz,0.0));
    vec3 worldView = normalize( WorldCameraPosition - worldPos );
    float Eta = 0.94;
    ReflectDir = reflect(-worldView, worldNorm );
    RefractDir = refract(-worldView, worldNorm, Eta);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
}