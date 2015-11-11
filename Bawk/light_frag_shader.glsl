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

/*vec4 get_point_light(vec3 worldpos, vec3 normal) {
    vec3 LightDirection = WorldPos - gPointLight.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);
    
    vec4 Color = CalcLightInternal(gPointLight.Base, LightDirection, WorldPos, Normal);
    
    float Attenuation =  gPointLight.Atten.Constant +
    gPointLight.Atten.Linear * Distance +
    gPointLight.Atten.Exp * Distance * Distance;
    
    Attenuation = max(1.0, Attenuation);
    
    return Color / Attenuation;
}*/

void main(void) {
    vec2 texcoord = get_tex_coord();
    // use the worldpos to do lighting calculations
    vec3 worldpos = texture(g_position_map, texcoord).xyz;
    vec3 color_o = texture(g_color_map, texcoord).xyz;
    vec3 color_t = texture(g_color_t_map, texcoord).xyz;
    vec4 color;
    color.xyz = (color_o + color_t) / 2.0;
    color.w = 1.0;
    //vec3 normal = texture(gNormalMap, texcoord).xyz;
    //normal = normalize(normal);
    
    if (l_draw_mode == 0) {
        // ambient lighting
        out_color = color;
    }
    else if (l_draw_mode == 1) {
        // point light
        // out_color = vec4(color, 1.0) * get_point_light(worldpos, normal);
        out_color = color;
    }
    else {
        // directional light or whatnot
        out_color = color;
    }
    //out_color.w = 1.0;
    //out_color = vec4(0.5,0.6,0.3,0.4);
}
