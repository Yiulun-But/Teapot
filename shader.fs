#version 330 core

layout(location = 0) out vec4 color;

uniform mat4 mv;

in vec2 texCoord;
in vec3 mv_pos3;
in vec3 mv_nml3;

void main()
{
    vec3 li = vec3(1.0, 1.0, 1.0); 
    vec3 k_s = vec3(1.0, 1.0, 1.0); 
    vec3 ld = vec3(0.0, 0.0, -1.0);  

    mat4 mv_s = mv;
    vec3 ld3 = normalize((mat3(mv) * ld));
    vec3 nml3 = normalize(mv_nml3);

    float cos_theta = max(dot(ld3, nml3), 0.0);

    vec3 r = - ld3 + 2.0 * cos_theta * nml3;

    vec3 v = vec3(0.0,0.0,-1.0);

    float cos_thy = max(dot(r, v), 0.0);

    vec4 k_d4 = vec4(1.0, 0.0, 0.0, 1.0);

    color = vec4(li * (cos_theta * k_d4.xyz + k_s * pow(cos_thy, 4.0)), 1.0);
}