#version 330

in vec3 g_coord;
in vec3 g_texturecoord;

//layout (location = 0) in vec3 coord;
//layout (location = 1) in vec3 texture_coord;

// in vec3 normal;

uniform mat4 g_mvp;
uniform mat4 g_worldtransform;

out vec3 g_worldpos;
out vec3 g_texcoord;
out vec3 gv_coord;

void main(void) {
    //normal_dir = (g_world_transform * vec4(normal, 0)).xyz;
    g_worldpos = (g_worldtransform * vec4(g_coord, 1)).xyz;
    //g_world_pos = g_coord;
    g_texcoord = g_texturecoord;
    gv_coord = g_coord;
    gl_Position = g_mvp * vec4(g_coord, 1);
}
