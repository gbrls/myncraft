#version 450 core

in vec3 TexCoord;
out vec4 outColor;

uniform sampler2DArray tex1;
uniform float percentage;

void main() {
    vec4 wirecolor = vec4(0.0, 1.0, 0.0, 0.4);

    vec4 idx_color = texture(tex1, TexCoord);
    outColor = mix(idx_color, wirecolor, percentage);
}

//
