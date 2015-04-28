//varying vec3 texCoords;
//
//void main( void )
//{
//    texCoords = normalize(gl_Vertex.xyz );
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//}



varying vec3 ReflectDir;

vec3 WorldCameraPosition;
uniform mat4 ModelView;

void main()
{
    mat4 tmp = mat4(1.0);
    WorldCameraPosition = vec3(0, 0, 10);
    //vec3 worldPos = vec3(ModelView * vec4(gl_Vertex.xyz,1.0));
    //vec3 worldNorm = vec3(ModelView * vec4(gl_Normal.xyz,0.0));
    vec3 worldPos = vec3(tmp * vec4(gl_Vertex.xyz,1.0));
    vec3 worldNorm = vec3(tmp * vec4(gl_Normal.xyz,0.0));
    vec3 worldView = normalize( WorldCameraPosition - worldPos );
        
    ReflectDir = reflect(-worldView, worldNorm );
 
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz,1.0);
}