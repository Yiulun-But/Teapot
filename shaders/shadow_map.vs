#version 330

layout(location=0) in vec3 pos;

uniform mat4 smvp;

void main() {
    gl_Position = smvp * vec4(pos, 1.0); 
}
