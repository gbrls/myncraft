#version 330

in vec2 position;
in vec3 color;

out vec3 Color;

void main() {
    Color = color;
    gl_Position = vec4(position, 1.0, 1.0);
}

//
