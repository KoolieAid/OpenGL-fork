#version 330 core 

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTex;

uniform mat4 transform;
uniform mat4 project;
uniform mat4 view;

out vec2 texCoord;

void main() {
	gl_Position = project * view * transform * vec4(aPos, 1.0);
	texCoord = aTex;
}