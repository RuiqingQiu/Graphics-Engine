uniform sampler2DShadow ShadowMap;

varying vec3 Position;
varying vec3 Normal;
varying vec4 ShadowCoord;

uniform int pass;

vec3 phongModelDiffAndSpec()
{
    vec3 n = Normal;
    if( !gl_FrontFacing ) n = -n;
    vec3 s = normalize(vec3(Light.Position) - Position);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
        pow( max( dot(r,v), 0.0 ), Material.Shininess );
    
    return diffuse + spec;
}


void shadeWithShadow()
{
    vec3 ambient = Light.Intensity * Material.Ka;
    vec3 diffAndSpec = phongModelDiffAndSpec();
    
    float shadow = textureProj(ShadowMap, ShadowCoord);
    
    // If the fragment is in shadow, use ambient light only.
    gl_FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);
    
    // Gamma correct
    gl_FragColor = pow( FragColor, vec4(1.0 / 2.2) );
}

void recordDepth()
{
    // Do nothing, depth will be written automatically
}

void main() {
    // This will call either shadeWithShadow or recordDepth
    if(pass == 1){
        recordDepth();
    }
    else{
        shadeWithShadow();
    }
}