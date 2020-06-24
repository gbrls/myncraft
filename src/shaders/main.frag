#version 330

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D texBlock;

void main() {

    //outColor = vec4(1.0, 1.0, 1.0, 1.0);

    vec2 coord = TexCoord * 1.;

    outColor = texture(texBlock, (coord));

}

//
