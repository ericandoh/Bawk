#version 120

attribute vec3 coord;
attribute vec3 texture_coord;
uniform mat4 mvp;
varying vec3 texcoord;

void main(void) {
    texcoord = texture_coord;
    gl_Position = mvp * vec4(coord, 1);
}
