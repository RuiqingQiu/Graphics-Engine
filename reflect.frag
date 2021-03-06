uniform sampler2D tex;
uniform sampler2D norm;
uniform sampler2D gloss;
uniform sampler2D metallic;

uniform samplerCube cubeMap;

varying vec3 ReflectDir;
varying vec2 TexCoords;


varying vec4 Position;
varying vec3 Normal;

varying vec3 Tangent;

varying vec3 LightDir;
varying vec3 ViewDir;

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

//void main() {
//    ReflectFactor = 0.3;
//    MaterialColor = vec4(0.5, 0.5, 0.5, 0.5);
//    // Access the cube map texture
//    vec4 base_color = texture2D(tex, TexCoords);
//    vec4 cubeMapColor = textureCube(cubeMap, ReflectDir);
//    gl_FragColor = mix(base_color, cubeMapColor, ReflectFactor);
//    //gl_FragColor = mix( MaterialColor, cubeMapColor, ReflectFactor);
//}



void main() {
    ReflectFactor = 0.3;
    vec4 normal = 2.0 * texture2D(norm, TexCoords) - 1.0;
    vec4 texColor = texture2D(tex, TexCoords);
    vec4 shadeColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);
    //gl_FragColor = shadeColor;
    vec4 cubeMapColor = textureCube(cubeMap, ReflectDir);
    gl_FragColor = mix(shadeColor, cubeMapColor, ReflectFactor);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(Tangent,1.0);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //gl_FragColor = mix(color, shadeColor, fogFactor);
}