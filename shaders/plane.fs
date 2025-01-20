#version 330 core

layout(location = 0) out vec4 color;

uniform mat4 v;
uniform mat4 mv;

uniform sampler2DShadow depthMap;

// Lighting
struct SpotLight {
    vec3 li_pos;
    vec3 li_dir;
    vec3 color;
    float radius;
    float cutoff;
};
uniform SpotLight spotlight;

// Light
struct Light {
    vec3 ray_dir;
    vec3 atten;
};

Light spot_light_intensity(SpotLight spotlight, vec3 pos) {
    vec3 light_pos = (v * vec4(spotlight.li_pos, 1.0)).xyz;
    vec3 light_dir = normalize((v * vec4(spotlight.li_dir, 0.0)).xyz);
    vec3 ray_dir = normalize(pos - light_pos);
    float cos = dot(light_dir, ray_dir);

    Light li;

    if (cos > spotlight.cutoff) {
        float d = length(pos - light_pos) / 15.0;
        vec3 atten =  spotlight.color * (2.0 / pow(spotlight.radius, 2.0)) * (1.0 - (d / sqrt(pow(d, 2.0) + pow(spotlight.radius, 2.0))));
        li.ray_dir = ray_dir;
        float intense = 1 - ((1 - cos) / (1 - spotlight.cutoff));
        li.atten = atten * intense;
        return li;
    }
    else {
        li.ray_dir = ray_dir;
        li.atten = vec3(0.0, 0.0, 0.0);
        return li;
    }
}


in vec3 pos3;
in vec4 lightPos;

void main() {

    vec3 pos = (mv * vec4(pos3, 1.0)).xyz;

    // Blinn shading
    Light li = spot_light_intensity(spotlight, pos);
    li.atten *= textureProj(depthMap, lightPos);
    vec3 ld3 = normalize(li.ray_dir);
    vec3 nml3 = vec3(0.0, 1.0, 0.0);
    nml3 = normalize((mat3(v) * nml3));

    float cos_theta = dot(-ld3, nml3);
    vec3 r = reflect(ld3, nml3);

    vec3 view = vec3(0.0,0.0,1.0);

    vec4 k_d4 = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 k_s4 = vec4(1.0, 1.0, 1.0, 1.0);

    vec4 k_a4 = vec4(0.1, 0.1, 0.1, 1.0);

    vec4 blin_shade = vec4(li.atten * (cos_theta * k_d4.xyz), 1.0) + k_a4;

    color = blin_shade;

}