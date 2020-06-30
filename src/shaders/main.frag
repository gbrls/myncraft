#version 450 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2DArray tex1;
uniform float percentage;

void main() {

    vec4 wirecolor = vec4(0.0, 1.0, 0.0, 0.4);
    vec2 coord = TexCoord * 1.;

    vec3 coord_3d = vec3(coord, 2);

    vec4 idx_color = texture(tex1, coord_3d);
    outColor = mix(idx_color, wirecolor, percentage);
}

//
