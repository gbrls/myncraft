#version 450 core

in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D texBlock;
//uniform sampler2DArray texArray;
uniform float percentage;

void main() {

    vec4 wirecolor = vec4(0.0, 1.0, 0.0, 0.4);
    vec2 coord = TexCoord * 1.;

    //vec3 coord_3d = vec3(coord, 0.0);

    //vec4 idx_color = texture(texArray, coord_3d);
    //vec4 idx_color = texture(texArray, vec3(0,0,0));

    // repeating textures;
    //coord.y *= 2;
    //coord.x *= 2;

    outColor = mix(texture(texBlock, (coord)), wirecolor, percentage);

    //outColor = mix(c, idx_color, 0.5);
    //outColor = mix(idx_color, wirecolor, percentage);
}

//
