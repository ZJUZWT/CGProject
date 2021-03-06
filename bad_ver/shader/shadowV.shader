#version 450 core

layout(location = 0) in vec4 position;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
	gl_Position = P * V * M * position;
}