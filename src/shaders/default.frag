#version 450 core

in vec2 TexCoord;

uniform sampler2D tex;
out vec4 outColor;

void main() {
    vec4 texColor = texture(tex, TexCoord);
    if(texColor.a < 0.1)
        discard;

    //texColor.g = max(texColor.g, 0.5f);

    outColor = texColor;
}

//
