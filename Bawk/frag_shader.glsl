#version 120

varying vec3 texcoord;

void main(void) {
    gl_FragColor = vec4(texcoord.x / 255.0, texcoord.y / 255.0, texcoord.z / 255.0, 1.0);
    //gl_FragColor = vec4(0.5, 0.4, 0.6, 1.0);
}
