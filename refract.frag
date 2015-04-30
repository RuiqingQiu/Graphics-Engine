varying vec3 ReflectDir;
varying vec3 RefractDir;

uniform samplerCube cubeMap;

float ReflectFactor;
vec4 MaterialColor;

void main() {
    ReflectFactor = 0.1;
    MaterialColor = vec4(0.5, 0.5, 0.5, 0.5);
    vec4 reflectColor = textureCube(cubeMap, ReflectDir);
    vec4 refractColor = textureCube(cubeMap, RefractDir);
    gl_FragColor = mix(refractColor, reflectColor, ReflectFactor);
}