#version 330

in vec3 l_coord;
uniform mat4 l_mvp;

void main(void) {
    gl_Position = l_mvp * vec4(l_coord, 1);
}
