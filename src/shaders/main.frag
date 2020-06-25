#version 330

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D texBlock;
uniform float percentage;

void main() {

    vec4 wirecolor = vec4(1.0, 1.0, 1.0, 1.0);

    vec2 coord = TexCoord * 1.;

    vec4 c = mix(texture(texBlock, (coord)), wirecolor, percentage);
    c.a = 0.2;

    outColor = c;

}

//
