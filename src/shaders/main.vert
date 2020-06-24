#version 330

in vec3 position;
in vec2 texcoord;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    TexCoord = texcoord;
}

//
