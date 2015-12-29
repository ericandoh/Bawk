#version 330

in vec3 g_worldpos;
in vec3 g_texcoord;
in vec3 gv_coord;

uniform sampler2D tile_texture;
uniform int g_drawmode;
uniform float g_intensity;
uniform float g_alphacutoff;
uniform int g_enable_shadows;

layout (location = 0) out vec3 worldpos_out;
layout (location = 1) out vec3 color_out;
layout (location = 3) out vec3 normal_out;

void main(void) {
    if (g_enable_shadows == 0) {
        worldpos_out = vec3(0,0,0);
        normal_out.xyz = vec3(0,0,0);
    }
    else {
        worldpos_out.xyz = g_worldpos;
        // fill in normal later on
        normal_out.xyz = vec3(0.3,0.4,0.5);
    }
    
    if (g_drawmode == 0) {
        vec4 color = texture(tile_texture, g_texcoord.xy);
        if(color.a < g_alphacutoff)
            discard;
        color.xyz *= g_intensity;
        color_out = color.xyz;
    }
    else if (g_drawmode == 1) {
        int shaderflags = int(g_texcoord.z);
        vec2 imgcoord;
        float intensity;
        if (mod(shaderflags, 0x2) > 0) {
            // we have a top or bottom face (texture_coord.z = 1)
            imgcoord = vec2((fract(gv_coord.x)/1.02+0.01 + g_texcoord.x) / 16.0, 1.0 - (fract(gv_coord.z)/1.02+0.01 + g_texcoord.y) / 16.0);
            intensity = 1.0;
        } else {
            // we have a side face
            imgcoord = vec2((fract(gv_coord.x + gv_coord.z)/1.02+0.01 + g_texcoord.x) / 16.0, 1.0 - (fract(gv_coord.y)/1.02+0.01 + g_texcoord.y) / 16.0);
            intensity = 0.85;
        }
        vec4 color = texture(tile_texture, imgcoord);
        color.xyz *= intensity * g_intensity;
        if(color.a < g_alphacutoff)
            discard;
        color_out = color.xyz;
    }
    else {
        color_out = g_texcoord.xyz * g_intensity;
    }
}
