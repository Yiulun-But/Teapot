#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 tex;
layout(location=2) in vec3 norm;

uniform mat4 mvp;
uniform mat3 mn;

out vec2 texCoord;
out vec3 mv_nml3;
out vec3 pos3;
out vec4 lightPos;

uniform mat4 TeapotShadowMatrix;

void main()
{
    texCoord = vec2(tex.x, tex.y);
    mv_nml3 = normalize(mn * norm);
    lightPos = TeapotShadowMatrix * vec4(pos, 1.0);
    pos3 = pos;

    gl_Position = mvp * vec4(pos, 1.0);

}