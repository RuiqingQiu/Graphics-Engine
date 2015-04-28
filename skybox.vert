varying vec3 texCoords;

void main( void )
{
    texCoords = normalize(gl_Vertex.xyz );
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
