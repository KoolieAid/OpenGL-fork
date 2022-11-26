#version 330 core

uniform sampler2D tex0;
uniform sampler2D tex1;

in vec2 texCoord;
out vec4 FragColor;

void main() {	
	FragColor = texture(tex0, texCoord);
}