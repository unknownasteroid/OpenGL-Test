#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 aColor;

out vec4 vColor;

void main() {
    gl_Position = position;
    vColor = aColor;
}