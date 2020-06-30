#version 450 core

in vec3 TexCoord;
in float Normal;
in float Light;
out vec4 outColor;

uniform sampler2DArray tex1;
uniform float percentage;
uniform vec3 Sun;

void main() {
    vec4 wirecolor = vec4(0.0, 1.0, 0.0, 0.4);
    vec4 shadow = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 norm = vec3(1, 0, 0);
    vec3 sun = normalize(vec3(0.3, -1, 0.1));

    float n = Normal;

    if(Normal < 1) { // front face
       norm = vec3(0, 0, 1);
    } else if(Normal < 2) { // left face
        norm = vec3(-1, 0, 0);
    } else if(Normal < 3) { // back
        norm = vec3(0, 0, -1);
    } else if(Normal < 4) { // right
         norm = vec3(1, 0, 0);
    } else if(Normal < 5) { // top
         norm = vec3(0, 1, 0);
    } else if(Normal < 6) { // bot
        norm = vec3(0, -1, 0);
    }

    float angle = clamp(dot(norm, sun), 0, 1);

    angle = min(angle, 0.7);

    vec4 idx_color = texture(tex1, TexCoord);
    idx_color = mix(idx_color, shadow, angle);


    float l = clamp(Light*2, 3, 10) * 0.1;
    vec4 c = mix(idx_color, wirecolor, percentage);

    outColor = vec4(c.rgb, c.a);
}

//
