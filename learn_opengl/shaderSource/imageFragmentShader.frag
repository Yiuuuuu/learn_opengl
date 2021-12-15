#version 440 core

in vec2 textureCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main(){
	FragColor = texture(texture1, textureCoord);
}