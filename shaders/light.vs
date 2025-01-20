#version 330 core

layout(location=0) in vec3 pos;

uniform mat4 lm;

void main()
{
    gl_Position = lm * vec4(pos, 1.0);
}