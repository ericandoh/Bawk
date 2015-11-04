#version 150

in vec3 v_coord;
in vec3 texcoord;
uniform sampler2D tile_texture;
uniform int draw_mode;
uniform float shade_intensity;
uniform float alpha_cutoff;
uniform float alpha_set;

out vec4 color;

void main(void) {
    if (draw_mode == 0) {
        // use the texture coordinates given to us directly
        color = texture(tile_texture, texcoord.xy);
        if(color.a < alpha_cutoff)
            discard;
        color.xyz *= shade_intensity;
        color.w = alpha_set;
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
        
        color = texture(tile_texture, imgcoord);
        if(color.a < alpha_cutoff)
            discard;
        color.xyz *= intensity * shade_intensity;
        color.w = alpha_set;
    }
    else {
        color = vec4(texcoord.xyz, alpha_set);
    }
}
