#version 330

uniform sampler2D g_position_map;
uniform sampler2D g_color_map;
uniform sampler2D g_color_t_map;
uniform sampler2D g_normal_map;

uniform vec2 l_screen_size;

// this is direction for directional light, or position for point light
uniform vec3 l_val;
// whether we draw directional or point light
uniform int l_draw_mode;

out vec4 out_color;

vec2 get_tex_coord() {
    return gl_FragCoord.xy / l_screen_size;
}

float get_point_light(vec3 worldpos) {
    float peak = 10;
    vec3 lightdir = worldpos - l_val;
    float distance = length(lightdir);
    if (distance > peak) {
        return 0;
    }
    // fraction will be greater the closer you are
    float fraction = 1.0 - distance / peak;
    return fraction * fraction * 1.0;
    
    //return 1;
    /*
    lightdir = normalize(lightdir);
    vec4 Color = CalcLightInternal(gPointLight.Base, LightDirection, WorldPos, Normal);
    float Attenuation =  gPointLight.Atten.Constant +
    gPointLight.Atten.Linear * Distance +
    gPointLight.Atten.Exp * Distance * Distance;
    Attenuation = max(1.0, Attenuation);
    return Color / Attenuation;*/
}

void main(void) {
    vec2 texcoord = get_tex_coord();
    // use the worldpos to do lighting calculations
    vec3 worldpos = texture(g_position_map, texcoord).xyz;
    vec3 color_o = texture(g_color_map, texcoord).xyz;
    vec3 color_t = texture(g_color_t_map, texcoord).xyz;
    vec3 color = (color_o + color_t) / 2.0;
    //vec3 normal = texture(gNormalMap, texcoord).xyz;
    //normal = normalize(normal);
    
    if (l_draw_mode == 0) {
        // ambient lighting
        out_color.xyz = color;
    }
    else if (l_draw_mode == 1) {
        // point light
        out_color.xyz = color * get_point_light(worldpos);
        // out_color = color;
    }
    else {
        // directional light or whatnot
        out_color.xyz = color;
    }
    out_color.w = 1.0;
}
