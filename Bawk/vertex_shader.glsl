#version 330

in vec3 coord;
in vec3 texture_coord;

//layout (location = 0) in vec3 coord;
//layout (location = 1) in vec3 texture_coord;

// in vec3 normal;

uniform mat4 mvp;
//uniform mat4 world_matrix;

out vec3 world_pos;
out vec3 texcoord;
out vec3 v_coord;

void main(void) {
    gl_Position = mvp * vec4(coord, 1);
    texcoord = texture_coord;
    // normal_dir = (world_matrix * vec4(normal, 0)).xyz;
    //world_pos = (world_matrix * vec4(coord, 1)).xyz;
    world_pos = coord;
    v_coord = coord;
}
