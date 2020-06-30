#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texcoord;
layout (location = 2) in float normal;
layout (location = 3) in float light;

out vec3 Color;
out vec3 TexCoord;
out float Normal;
out float Light;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    TexCoord = texcoord;
    Normal = normal;
    Light = light;
}

//
