#version 440 core

in vec2 textureCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main(){
	vec4 textureColor = texture(texture1, textureCoord);
	if (textureColor.a < 0.1)
		discard;
	FragColor = textureColor;
}