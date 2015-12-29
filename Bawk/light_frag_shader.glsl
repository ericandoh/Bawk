#version 330

uniform sampler2D g_position_map;
uniform sampler2D g_color_map;
uniform sampler2D g_color_t_map;
uniform sampler2D g_normal_map;

uniform sampler2D g_shadow_map;
uniform mat4 l_shadow_mvp;

uniform vec2 l_screen_size;

// this is direction for directional light, or position for point light
uniform vec3 l_val;
// some other misc info that we can pass in
uniform vec3 l_properties;
// whether we draw directional or point light
uniform int l_draw_mode;

out vec4 out_color;

vec2 poisson_disk[4] = vec2[](vec2( -0.94201624, -0.39906216 ),
                              vec2( 0.94558609, -0.76890725 ),
                              vec2( -0.094184101, -0.92938870 ),
                              vec2( 0.34495938, 0.29387760 ));

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
    if (l_draw_mode == 1) {
        // ambient lighting, do lighting here
        
        vec4 shadow_coord = l_shadow_mvp * vec4(worldpos, 1);
        float bias = 0.0005;
        //float bias = 0;
        float visibility = 1.0;
        //if (texture(g_shadow_map, shadow_coord.xy).x < shadow_coord.z - bias) {
        //    visibility = 0.5;
        //}
        for (int i = 0; i < 4; i++) {
            if (texture(g_shadow_map, shadow_coord.xy + poisson_disk[i]/700.0).x < shadow_coord.z-bias) {
                visibility-=0.125;
            }
        }
        
        //float visibility = 1.0 - texture(g_shadow_map, vec3(shadow_coord.xy, (shadow_coord.z - bias) / shadow_coord.w ));
        out_color.xyz = out_color.xyz * l_properties.y * visibility;
    }
    else if (l_draw_mode == 2) {
        // point light
        out_color.xyz = out_color.xyz * get_point_light(worldpos);
        // out_color = color;
    }
    //else {
        // directional light or whatnot
        // out_color.xyz = color_o;
    //}
    else if (l_draw_mode == 3) {
        // hack, but this is to show the light shader part
        float depth = texture(g_shadow_map, texcoord.xy).x;
        //float depth = 1.0 - texture(g_shadow_map, vec3(texcoord.xy, 0)).x;
        depth = 1.0 - (1.0 - depth) * 25.0;
        //depth = depth / 15.0;
        //depth = depth - 0.3;
        out_color = vec4(depth);
    }
    out_color.w = 1.0;
}
