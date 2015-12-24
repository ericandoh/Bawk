#version 330

in vec3 g_coord;

uniform mat4 g_mvp;

void main(void) {
    gl_Position = g_mvp * vec4(g_coord, 1);
}
