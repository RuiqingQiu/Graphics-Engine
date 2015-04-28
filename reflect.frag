varying vec3 ReflectDir;

uniform samplerCube cubeMap;

float ReflectFactor;
vec4 MaterialColor;

void main() {
    ReflectFactor = 0.85;
    MaterialColor = vec4(0.5, 0.5, 0.5, 0.5);
    // Access the cube map texture
    vec4 cubeMapColor = textureCube(cubeMap, ReflectDir);

    gl_FragColor = mix( MaterialColor, cubeMapColor, ReflectFactor);
}