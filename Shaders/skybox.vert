#version 330 core 

layout(location = 0) in vec3 aPos;

uniform mat4 project;
uniform mat4 view;

out vec3 texCoord;

void main() {
	vec4 pos = project * view * vec4(aPos, 1.0);
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
	texCoord = aPos;
}