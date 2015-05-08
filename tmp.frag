uniform sampler2D ShadowMap;

varying vec4 ShadowCoord;
varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;

uniform vec4 LightPosition;

void phongModel(vec4 position, vec3 normal, out vec3 ambient, out vec3 diffuse_spec){
    vec3 Kd = vec3(0.64, 0.64, 0.64);
    vec3 Ld = vec3(1.0, 1.0, 1.0);
    vec3 Ka = vec3(0.0, 0.0, 0.0);
    vec3 La = vec3(0.4, 0.4, 0.4);
    vec3 Ks = vec3(0.5, 0.5, 0.5);
    vec3 Ls = vec3( 1.0, 1.0, 1.0);
    float Shininess = 100.0;
    
    vec3 n = normalize(normal);
    vec3 s = normalize(vec3(LightPosition - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect(-s, n);
    ambient = La * Ka;
    float sDotN = max( dot(s,normal), 0.0 );
    vec3 diffuse = Ld * gl_Color.rgb * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Ls * Ks * pow( max( dot(r,v), 0.0 ), Shininess );
    diffuse_spec = diffuse + spec;
}

void main()
{
    vec3 ambient;
    vec3 diffuse_spec;
    phongModel(Position, Normal, ambient, diffuse_spec);
    vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
    
    // Used to lower moirÃ© pattern and self-shadowing
    shadowCoordinateWdivide.z += 0.0005;
    
    
    float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
    
    
    float shadow = 1.0;
    if (ShadowCoord.w > 0.0)
        shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
    //gl_FragColor = shadow * vec4(1.0, 0.0, 0.0, 1.0);
    gl_FragColor = vec4(shadow * diffuse_spec + ambient, 1.0);
    
}
