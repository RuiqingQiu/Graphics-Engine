varying vec4 Position;
varying vec3 Normal;

uniform sampler2D RenderTex;
uniform int pass;

//float EdgeThreshold;
const vec3 lum = vec3(0.2126, 0.7152, 0.0722);
//
vec3 phongModel(vec4 position, vec3 normal){
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
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Ls * Ks * pow( max( dot(r,v), 0.0 ), Shininess );
    return ambient + diffuse + spec;
}

float threshold(in float thr1, in float thr2 , in float val) {
    if (val < thr1) {return 0.0;}
    if (val > thr2) {return 1.0;}
    return val;
}

// averaged pixel intensity from 3 color channels
float avg_intensity(in vec4 pix) {
    //return dot(lum, pix.rgb);
    return (pix.r + pix.g + pix.b)/3.;
}

vec4 get_pixel(in vec2 coords, in float dx, in float dy) {
    return texture2D(RenderTex,coords + vec2(dx, dy));
}

// returns pixel color
float IsEdge(in vec2 coords){
    float dxtex = 1.0 / 512.0 /*image width*/;
    float dytex = 1.0 / 512.0 /*image height*/;
    float pix[9];
    int k = -1;
    float delta;
    
    // read neighboring pixel intensities
    for (int i=-1; i<2; i++) {
        for(int j=-1; j<2; j++) {
            k++;
            pix[k] = avg_intensity(get_pixel(coords,float(i)*dxtex,
                                             float(j)*dytex));
        }
    }
    //float sx = pix[0] + 2.0 * pix[3] + pix[6] - (pix[2] + 2.0 * pix[4] + pix[8]);
    //float sy = pix[0] + 2.0 * pix[1] + pix[2] - (pix[6] + 2.0 * pix[7] + pix[8]);
    
    //float g = sx * sx + sy * sy;
    //return threshold(0.4, 0.4, g);
    
    // average color differences around neighboring pixels
    delta = (abs(pix[1]-pix[7])+
             abs(pix[5]-pix[3]) +
             abs(pix[0]-pix[8])+
             abs(pix[2]-pix[6])
             )/4.;
    
    return threshold(0.4,0.4,clamp(1.8*delta,0.0,1.0));
}

void main()
{
    float width = 512.0;
    float height = 512.0;
    if(pass == 1){
        gl_FragColor = vec4(phongModel(Position, Normal),1.0);
    }
    else{
        vec4 color = vec4(0.0,0.0,0.0,1.0);
        vec2 pix = vec2(gl_FragCoord.x/width, gl_FragCoord.y /height);
        color.g = IsEdge(pix);
        //gl_FragColor = color;
        if(color.g == 0.0){
            gl_FragColor = texture2D(RenderTex, pix);
        }
        else{
            gl_FragColor = color;
        }
        
    }
}




