uniform samplerCube cubeMap;
varying vec3 texCoords;

void main( void )
{
    gl_FragColor = textureCube(cubeMap, texCoords );
}