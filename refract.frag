uniform sampler2D tex;
uniform sampler2D norm;
uniform sampler2D gloss;
uniform sampler2D metallic;

uniform samplerCube cubeMap;

varying vec2 TexCoords;
varying vec4 Position;
varying vec3 Normal;

varying vec3 Tangent;

varying vec3 LightDir;
varying vec3 ViewDir;

varying vec3 ReflectDir;
varying vec3 RefractDir;


float ReflectFactor;
vec4 MaterialColor;

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

void main() {
    ReflectFactor = 0.1;
    MaterialColor = vec4(0.5, 0.5, 0.5, 0.5);
    vec4 reflectColor = textureCube(cubeMap, ReflectDir);
    vec4 refractColor = textureCube(cubeMap, RefractDir);
    vec4 mix_color = mix(refractColor, reflectColor, ReflectFactor);
    
    vec4 normal = 2.0 * texture2D(norm, TexCoords) - 1.0;
    vec4 texColor = texture2D(tex, TexCoords);
    vec4 shadeColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);
    //gl_FragColor = mix_color;
    float MixFactor = 0.2;
    gl_FragColor = mix(shadeColor, mix_color, MixFactor);

    
}