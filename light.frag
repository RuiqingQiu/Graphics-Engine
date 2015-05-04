uniform sampler2D tex;
uniform sampler2D norm;
uniform sampler2D gloss;
uniform sampler2D metallic;
varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;

varying vec3 Tangent;

varying vec3 LightDir;
varying vec3 ViewDir;
varying float height_pass;

//Fog Attributes
float maxDist = 150.0;
float minDist = 10.0;
vec4 color = vec4(0.5, 0.5, 0.5, 1.0);

/*
void phongModel(vec4 position, vec3 normal, out vec3 ambAndDiff, out vec3 spec) {
    vec3 Kd = vec3(0.64, 0.64, 0.64);
    vec3 Ld = vec3(1.0, 1.0, 1.0);
    vec3 Ka = vec3(0.0, 0.0, 0.0);
    vec3 La = vec3(0.4, 0.4, 0.4);
    vec3 Ks = vec3(0.5, 0.5, 0.5);
    vec3 Ls = vec3( 1.0, 1.0, 1.0);
    float Shininess = 100.0;
    vec4 LightPosition = vec4(0.0, 10.0, 20.0, 1.0);
    
    vec3 n = normalize(normal);
    vec3 s = normalize(vec3(LightPosition - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect(-s, n);
    vec3 ambient = La * Ka;
    float sDotN = max( dot(s,normal), 0.0 );
    vec3 diffuse = Ld * Kd * sDotN;
    spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Ls * Ks * pow( max( dot(r,v), 0.0 ), Shininess );
    ambAndDiff = ambient + diffuse;
}
*/

vec3 phongModel(vec3 normal, vec3 diffR){
    vec3 Kd = vec3(0.64, 0.64, 0.64);
    vec3 Ld = vec3(1.0, 1.0, 1.0);
    vec3 Ka = vec3(0.0, 0.0, 0.0);
    vec3 La = vec3(0.4, 0.4, 0.4);
    vec3 Ks = vec3(0.5, 0.5, 0.5);
    vec3 Ls = vec3( 1.0, 1.0, 1.0);
    float Shininess = 100.0;
    vec4 LightPosition = vec4(0.0, 10.0, 20.0, 1.0);
    vec3 Intensity = vec3(0.9,0.9,0.9);
    
    
    vec3 r = reflect(-LightDir, normal);
    vec3 ambient = Intensity * Ka;
    float sDotN = max(dot(LightDir, normal), 0.0);
    
    vec3 diffuse = Intensity * diffR * sDotN;
    
    vec3 spec = vec3(0.0);
    if(sDotN > 0.0){
        spec = Intensity * Ks * pow(max(dot(r, ViewDir), 0.0), Shininess);
    }
    
    return ambient + diffuse + spec;
}

vec3 CalcBumpedNormal(){
    vec3 normal_tmp = normalize(Normal);
    vec3 tangent_tmp = normalize(Tangent);
    tangent_tmp = normalize(tangent_tmp-dot(tangent_tmp, normal_tmp) * normal_tmp);
    vec3 Bitangent = cross(tangent_tmp, normal_tmp);
    vec3 BumpMapNormal = texture2D(norm, TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0,1.0,1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(tangent_tmp, Bitangent, normal_tmp);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

void main() {
    //vec3 normal = CalcBumpedNormal();
    //gl_FragColor = vec4(normal, 1.0);
    vec4 normal = 2.0 * texture2D(norm, TexCoords) - 1.0;
    //gl_FragColor = normal;
    vec4 texColor = texture2D(tex, TexCoords);
    //gl_FragColor = texColor;
    //gl_FragColor = vec4(Tangent,1.0);
    float dist = length(Position.xyz);
    float fogFactor = (maxDist - dist)/(maxDist-minDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    //gl_FragColor = vec4(height_pass, 0.0,0.0,1.0);
    vec3 ambAndDiff, spec;
    vec4 shadeColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);
    gl_FragColor = shadeColor;
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(Tangent,1.0);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //gl_FragColor = mix(color, shadeColor, fogFactor);
}