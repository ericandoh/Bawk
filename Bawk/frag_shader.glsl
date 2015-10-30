#version 120

varying vec3 v_coord;
varying vec3 texcoord;
uniform sampler2D tile_texture;
uniform int draw_mode;
uniform float shade_intensity;

void main(void) {
    if (draw_mode == 0) {
        // use the texture coordinates given to us directly
        gl_FragColor = texture2D(tile_texture, texcoord.xy);
    }
    else if (draw_mode == 1) {
        int shaderflags = int(texcoord.z);
        vec2 imgcoord;
        float intensity;
        if (mod(shaderflags, 0x2) > 0) {
            // we have a top or bottom face (texture_coord.z = 1)
            imgcoord = vec2((fract(v_coord.x) + texcoord.x) / 16.0, 1.0 - (fract(v_coord.z) + texcoord.y) / 16.0);
            intensity = 1.0;
        } else {
            // we have a side face
            imgcoord = vec2((fract(v_coord.x + v_coord.z) + texcoord.x) / 16.0, 1.0 - (fract(v_coord.y) + texcoord.y) / 16.0);
            intensity = 0.85;
        }
        
        //if (mod(shaderflags / 0x2, 0x2) > 0) {
        //    intensity = intensity / 3.0;
        //}
        
        vec4 color = texture2D(tile_texture, imgcoord);
        if(color.a < 0.4)
            discard;
        color.xyz *= intensity * shade_intensity;
        gl_FragColor = color;
    }
    else {
        gl_FragColor = vec4(texcoord.xyz, 1.0) * shade_intensity;
    }
}
