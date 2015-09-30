#version 120

varying vec4 texcoord;

void main(void) {
    gl_FragColor = vec4(texcoord.w / 64.0, texcoord.w / 128.0, texcoord.w / 256.0, 1.0);
    //gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
