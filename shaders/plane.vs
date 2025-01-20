#version 330 core

layout(location=0) in vec3 pos;

uniform mat4 pm;
uniform mat4 shadowMatrix;

out vec3 pos3;
out vec4 lightPos;

void main()
{
    pos3 = pos;
    lightPos = shadowMatrix * vec4(pos, 1.0);
    gl_Position = pm * vec4(pos, 1.0);
}