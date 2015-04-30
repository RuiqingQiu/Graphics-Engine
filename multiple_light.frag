uniform sampler2D tex;
uniform sampler2D norm;
uniform sampler2D gloss;
uniform sampler2D metallic;

varying vec4 Position;
varying vec3 Normal;
varying vec2 TexCoords;

//varying vec3 Tangent;
//varying vec3 LightDir;
//varying vec3 ViewDir;

//Fog Attributes
float maxDist = 150.0;
float minDist = 10.0;
vec4 color = vec4(0.5, 0.5, 0.5, 1.0);

//Light Attributes
vec3 Kd = vec3(0.64, 0.64, 0.64);
vec3 Ld = vec3(1.0, 1.0, 1.0);
vec3 Ka = vec3(0.0, 0.0, 0.0);
vec3 La = vec3(0.4, 0.4, 0.4);
vec3 Ks = vec3(0.5, 0.5, 0.5);
vec3 Ls = vec3( 1.0, 1.0, 1.0);
float Shininess = 100.0;
vec4 LightPosition = vec4(0.0, 10.0, 20.0, 1.0);
vec4 lights[3];

void phongModel(vec4 position, vec3 normal, out vec3 ambAndDiff, out vec3 spec) {
    lights[0] = vec4(0.0, 10.0, 20.0, 1.0);
    lights[1] = vec4(5.0, 0.0, 0.0, 1.0);
    lights[2] = vec4(-5.0, 0.0, -10.0, 1.0);
    spec = vec3(0.0);
    for(int i = 0; i < 1; i++){
        vec3 n = normalize(normal);
        vec3 s = normalize(vec3(lights[i] - position));
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s, n);
        vec3 ambient = La * Ka;
        float sDotN = max( dot(s,normal), 0.0 );
        vec3 diffuse = Ld * Kd * sDotN;
        if( sDotN > 0.0 )
            spec += Ls * Ks * pow( max( dot(r,v), 0.0 ), Shininess );
        ambAndDiff += ambient + diffuse;
    }
}

void main() {
    vec4 texColor = texture2D(tex, TexCoords);
    gl_FragColor = texColor;
    float dist = length(Position.xyz);
    float fogFactor = (maxDist - dist)/(maxDist-minDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 ambAndDiff, spec;
    phongModel(Position, Normal, ambAndDiff, spec);
    vec4 shadeColor = vec4(ambAndDiff, 1.0) * texColor + vec4(spec, 1.0);
    //gl_FragColor = shadeColor;
    //gl_FragColor = mix(color, shadeColor, fogFactor);
}