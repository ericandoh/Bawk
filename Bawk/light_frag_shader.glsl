#version 150

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
//uniform sampler2D gNormalMap;

uniform vec2 gScreenSize;
uniform int draw_mode;

// this is direction for directional light, or position for point light
// uniform vec3 val;

out vec4 out_color;

vec2 get_tex_coord() {
    return gl_FragCoord.xy / gScreenSize;
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
    vec3 worldpos = texture(gPositionMap, texcoord).xyz;
    vec3 color = texture(gColorMap, texcoord).xyz;
    //vec3 normal = texture(gNormalMap, texcoord).xyz;
    //normal = normalize(normal);
    
    if (draw_mode == 0) {
        // ambient lighting
        out_color = color;
    }
    else if (draw_mode == 1) {
        // point light
        // out_color = vec4(color, 1.0) * get_point_light(worldpos, normal);
        out_color = color;
    }
    else {
        // directional light or whatnot
        out_color = color;
    }
}
