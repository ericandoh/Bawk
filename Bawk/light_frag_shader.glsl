#version 330

uniform sampler2D g_position_map;
uniform sampler2D g_color_map;
uniform sampler2D g_color_t_map;
uniform sampler2D g_normal_map;

uniform vec2 l_screen_size;

// this is direction for directional light, or position for point light
uniform vec3 l_val;
// some other misc info that we can pass in
uniform vec3 l_properties;
// whether we draw directional or point light
uniform int l_draw_mode;

out vec4 out_color;

vec2 get_tex_coord() {
    return gl_FragCoord.xy / l_screen_size;
}

float get_point_light(vec3 worldpos) {
    vec3 lightdir = worldpos - l_val;
    float distance = length(lightdir);
    if (distance > l_properties.x) {
        discard;
        //return 0;
    }
    // fraction will be greater the closer you are
    float fraction = 1.0 - distance / l_properties.x;
    return fraction * l_properties.y;
    
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
    //vec3 normal = texture(gNormalMap, texcoord).xyz;
    //normal = normalize(normal);
    
    out_color.xyz = (color_o.xyz + color_t) / 2.0;
    if (l_draw_mode == 0) {
        // ambient lighting
        out_color.xyz = out_color.xyz * 1.3f;
    } else if (l_draw_mode == 1) {
        // point light
        out_color.xyz = out_color.xyz * get_point_light(worldpos);
        // out_color = color;
    }
    //else {
        // directional light or whatnot
        // out_color.xyz = color_o;
    //}
    out_color.w = 1.0;
}
