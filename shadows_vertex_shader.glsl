#version 330

attribute in vec3 g_coord;
attribute in vec3 texture_coord;
uniform mat4 g_mvp;

void main(void) {
    gl_Position = g_mvp * vec4(g_coord, 1);
}
