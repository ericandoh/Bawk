#version 150

in vec3 coord;
in vec3 texture_coord;
uniform mat4 mvp;

out vec3 v_coord;
out vec3 texcoord;

void main(void) {
    v_coord = coord;
    texcoord = texture_coord;
    gl_Position = mvp * vec4(coord, 1);
}
