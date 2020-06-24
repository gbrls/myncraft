#version 330

in vec2 position;
in vec3 color;

out vec3 Color;

uniform mat4 view;
uniform mat4 proj;

void main() {
    Color = color;
    gl_Position = proj * view * vec4(position, 1.0, 1.0);
}

//
