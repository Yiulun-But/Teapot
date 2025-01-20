#version 330 core

layout(location = 0) out vec4 color;

uniform mat4 v;
uniform mat4 vp;
uniform sampler2D tex;
uniform sampler2D specTex;
uniform samplerCube ref;

in vec2 texCoord;
in vec3 mv_nml3;

void main()
{
    // Blinn shading
    vec3 li = vec3(0.1, 0.1, 0.1);  
    vec3 ld = vec3(-1.0, 1.0,1.0);  

    vec3 ld3 = normalize((mat3(v) * ld));
    vec3 nml3 = normalize(mv_nml3);

    float cos_theta = max(dot(-ld3, nml3), 0.0);
    vec3 r = reflect(ld3, nml3);

    vec3 v = vec3(0.0,0.0,1.0);

    float cos_thy = max(dot(r, v), 0.0);

    vec4 k_d4 = texture( tex, texCoord );
    vec4 k_s4 = texture( specTex, texCoord );

    vec4 blin_shade = vec4(li * (cos_theta * k_d4.xyz + k_s4.xyz * pow(cos_thy, 10.0)), 1.0);

    // Environment reflection
    vec3 e = reflect(-v, nml3);
    vec3 dir = (inverse(vp) * vec4(e, 1.0)).xyz;

    vec4 k_r = texture(ref, dir);

    color = blin_shade + k_r;
}