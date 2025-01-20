#version 330 core

layout(location = 0) out vec4 color;

uniform mat4 v;
uniform mat4 mv;
uniform sampler2D tex;
uniform sampler2D specTex;

in vec2 texCoord;
in vec3 mv_nml3;
in vec3 pos3;

// Material
struct Material {
    vec3 k_d; 
    vec3 k_s;    
    vec3 k_a;   
    float shine; 
};
uniform Material mtl; 

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
        float d = length(pos - light_pos) / 20.0;
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

uniform sampler2DShadow TeapotDepthMap;
in vec4 lightPos;

void main()
{
    vec3 pos = (mv * vec4(pos3, 1.0)).xyz;

    // Blinn shading
    Light li = spot_light_intensity(spotlight, pos);
    li.atten *= textureProj(TeapotDepthMap, lightPos);
    vec3 ld3 = normalize(li.ray_dir);
    vec3 nml3 = normalize(mv_nml3);

    float cos_theta = max(dot(-ld3, nml3), 0.0);
    vec3 r = reflect(ld3, nml3);

    vec3 view = normalize(-pos);

    float cos_thy = max(dot(r, view), 0.0);

    vec4 mk_d4 = texture( tex, texCoord );
    vec4 mk_s4 = texture( specTex, texCoord );

    vec3 diff = vec3(mk_d4) * mtl.k_d;
    vec3 spec = vec3(mk_s4) * mtl.k_s;
    vec3 t_k_a = mtl.k_a;

    // Temporary color
    vec3 diff_temp = vec3(128.0 / 255.0,0.0 / 255.0,0.0 / 255.0);
    vec3 spec_temp = vec3(0.8, 0.8, 0.8);
    vec3 amb_temp = diff_temp;

    vec4 l_a = vec4(0.1, 0.1, 0.1, 1.0);

    vec4 blin_shade = vec4(li.atten * (cos_theta * diff_temp + spec_temp * pow(cos_thy, mtl.shine)), 1.0) + vec4(amb_temp, 1.0) * l_a;

    color = blin_shade;
}