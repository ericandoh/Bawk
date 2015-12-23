#version 330

layout (location = 0) out float frag_depth;

void main(void) {
    frag_depth = gl_FragCoord.z;
}
