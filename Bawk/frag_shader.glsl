#version 120

varying vec3 v_coord;
varying vec3 texcoord;
uniform sampler2D tile_texture;

void main(void) {
    //gl_FragColor = vec4(0.5, 0.4, 0.6, 1.0);
    
    vec2 imgcoord;
    float intensity;
    
    if (texcoord.z > 0.0) {
        // we have a top or bottom face (texture_coord.z = 1)
        imgcoord = vec2((fract(v_coord.x) + texcoord.x) / 16.0, 1.0 - (fract(v_coord.z) + texcoord.y) / 16.0);
        intensity = 1.0;
    } else {
        // we have a side face
        imgcoord = vec2((fract(v_coord.x + v_coord.z) + texcoord.x) / 16.0, 1.0 - (fract(v_coord.y) + texcoord.y) / 16.0);
        intensity = 0.85;
    }
    
    vec4 color = texture2D(tile_texture, imgcoord);
    if(color.a < 0.4)
        discard;
    color.xyz *= intensity;
    gl_FragColor = color;
}
