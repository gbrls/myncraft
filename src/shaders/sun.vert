#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 offset;

void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    TexCoord = texcoord;
}

//
