varying vec4 Position;
varying vec3 Normal;

//for render pass
uniform int pass;
uniform sampler2D RenderTex;

uniform float width;
uniform float height;

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

vec4 get_pixel(in vec2 coords, in float dx, in float dy) {
    return texture2D(RenderTex,coords + vec2(dx, dy));
}

vec4 pass2()
{
    vec2 pix = vec2(gl_FragCoord.x/width, gl_FragCoord.y/height);
    vec4 sum = get_pixel(pix, 0.0, 0.0) * 0.2270270270;
    sum += get_pixel(pix, 0.0, 1.0) * 0.1945945946;
    sum += get_pixel(pix, 0.0, -1.0) * 0.1945945946;
    
    sum += get_pixel(pix, 0.0, 2.0) * 0.1216216216;
    sum += get_pixel(pix, 0.0, -2.0) * 0.1216216216;
    
    sum += get_pixel(pix, 0.0, 3.0) * 0.0540540541;
    sum += get_pixel(pix, 0.0, -3.0) * 0.0540540541;
    
    sum += get_pixel(pix, 0.0, 4.0) * 0.0162162162;
    sum += get_pixel(pix, 0.0, -4.0) * 0.0162162162;
    return sum;
    /*
    ivec2 pix = ivec2( gl_FragCoord.xy );
    vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,1) ) * Weight[1];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-1) ) * Weight[1];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,2) ) * Weight[2];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-2) ) * Weight[2];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,3) ) * Weight[3];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-3) ) * Weight[3];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,4) ) * Weight[4];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(0,-4) ) * Weight[4];
    return sum;
    */
}

vec4 pass3()
{
    vec2 pix = vec2(gl_FragCoord.x/width, gl_FragCoord.y /height);
    vec4 sum = get_pixel(pix, 0.0, 0.0) * 0.2270270270;
    sum += get_pixel(pix, 0.0, 1.0) * 0.1945945946;
    sum += get_pixel(pix, 0.0, -1.0) * 0.1945945946;
    
    sum += get_pixel(pix, 0.0, 2.0) * 0.1216216216;
    sum += get_pixel(pix, 0.0, -2.0) * 0.1216216216;
    
    sum += get_pixel(pix, 0.0, 3.0) * 0.0540540541;
    sum += get_pixel(pix, 0.0, -3.0) * 0.0540540541;
    
    sum += get_pixel(pix, 0.0, 4.0) * 0.0162162162;
    sum += get_pixel(pix, 0.0, -4.0) * 0.0162162162;
    return sum;
    /*
    ivec2 pix = ivec2( gl_FragCoord.xy );
    vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(1,0) ) * Weight[1];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(-1,0) ) * Weight[1];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(2,0) ) * Weight[2];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(-2,0) ) * Weight[2];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(3,0) ) * Weight[3];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(-3,0) ) * Weight[3];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(4,0) ) * Weight[4];
    sum += texelFetchOffset( Texture0, pix, 0, ivec2(-4,0) ) * Weight[4];
    return sum;
    */
    
}

void main()
{
    if(pass == 1){
        gl_FragColor = vec4(phongModel( Position, Normal ),1.0);
    }
    else if(pass == 2){
        gl_FragColor = pass2();
    }
    else{
        gl_FragColor = pass3();
    }
}