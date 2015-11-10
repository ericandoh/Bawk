#version 330

in vec3 world_pos;
in vec3 texcoord;
in vec3 v_coord;

uniform sampler2D tile_texture;
uniform int draw_mode;
/*
uniform float shade_intensity;
uniform float alpha_cutoff;
uniform float alpha_set;
*/
 
//out vec4 color;

//layout (location = 0)
//out vec3 WorldPosOut;
//out vec3 DiffuseOut;
//out vec3 NormalOut;
//out vec3 TexCoordOut;


layout (location = 0) out vec3 worldpos_out;
layout (location = 1) out vec3 color_out;
layout (location = 2) out vec3 color_out_t;
layout (location = 3) out vec3 normal_out;

void main(void) {
    worldpos_out = world_pos;
    // diffuse, wtf we don't need this shit
    // gl_FragData[1] = vec3(0.5,0.5,0.5);
    // normal
    //NormalOut = normalize(Normal0);
    
    
    //worldpos_out = vec3(0.5,0.5,0.1);
    //o_color = texcoord;
    color_out_t = vec3(0.1,0.1,0.1);
    normal_out = vec3(0.1,0.1,0.1);
    
    if (draw_mode == 0) {
        // use the texture coordinates given to us directly
        vec4 color = texture(tile_texture, texcoord.xy);
        //if(color.a < alpha_cutoff)
        //    discard;
        //color.xyz *= shade_intensity;
        color_out = color.xyz;
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
        vec4 color = texture(tile_texture, imgcoord);
        //if(color.a < alpha_cutoff)
        //    discard;
        //color.xyz *= intensity * shade_intensity;
        color_out = color.xyz;
    }
    else {
        color_out = texcoord.xyz;
    }
    
    //o_worldpos = vec3(0.4,0.4,0.4);
    //o_normal = vec3(0.5,0.5,0.5);
    //o_color = vec3(0.5,0.5,0.5);
    //o_color_t = vec3(1,1,1);
    //o_color_t = o_color;
}
