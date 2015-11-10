#version 330

in vec3 g_worldpos;
in vec3 g_texcoord;
in vec3 gv_coord;

uniform sampler2D tile_texture;
uniform int g_drawmode;
uniform float g_intensity;

layout (location = 0) out vec3 worldpos_out;
layout (location = 1) out vec3 color_out;
layout (location = 2) out vec3 color_out_t;
layout (location = 3) out vec3 normal_out;

void main(void) {
    worldpos_out = g_worldpos;
    // fill these in later on
    normal_out = vec3(0.3,0.4,0.5);
    
    if (g_drawmode == 0) {
        // use the texture coordinates given to us directly
        vec2 imgcoord = vec2(0.5,0.5);
        //vec4 color = texture(tile_texture, g_texcoord.xy);
        vec4 color = texture(tile_texture, imgcoord);
        color.xyz *= g_intensity;
        //if(color.a < 0.3)
        //    discard;
        color_out_t = color.xyz;
        //if(color.a > 0.7)
        color_out = color.xyz;
    }
    else if (g_drawmode == 1) {
        int shaderflags = int(g_texcoord.z);
        vec2 imgcoord;
        float intensity;
        if (mod(shaderflags, 0x2) > 0) {
            // we have a top or bottom face (texture_coord.z = 1)
            //imgcoord = vec2((fract(gv_coord.x) + g_texcoord.x) / 16.0, 1.0 - (fract(gv_coord.z) + g_texcoord.y) / 16.0);
            imgcoord = vec2((fract(gv_coord.x) + g_texcoord.x + 1) / 16.0, 1.0 - (fract(gv_coord.z) + g_texcoord.y + 6) / 16.0);
            intensity = 1.0;
        } else {
            // we have a side face
            imgcoord = vec2((fract(gv_coord.x + gv_coord.z) + g_texcoord.x) / 16.0, 1.0 - (fract(gv_coord.y) + g_texcoord.y) / 16.0);
            intensity = 0.85;
        }
        vec4 color = texture(tile_texture, imgcoord);
        color.xyz *= intensity;
        color.xyz *= g_intensity;
        //if(color.a < 0.3)
        //    discard;
        color_out_t = color.xyz;
        //if(color.a > 0.7)
        color_out = color.xyz;
    }
    else {
        color_out = g_texcoord.xyz * g_intensity;
        color_out_t = g_texcoord.xyz * g_intensity;
    }
}
