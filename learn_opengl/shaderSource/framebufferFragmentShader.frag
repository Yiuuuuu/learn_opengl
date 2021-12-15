#version 440 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main(){
	//-------------kernel-------------
	const float offset = 1.0/300.0;
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

	//---Types of kernel---
	//Sharpen
    float normal_kernel[9] = float[](
        0, 0, 0,
        0, 1, 0,
        0, 0, 0
    );
	//Sharpen
    float sharpen_kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
	//Blur
 	float blur_kernel[9] = float[](
 		2.0/16, 2.0/16, 2.0/16,
 		2.0/16, 4.0/16, 2.0/16,
 		2.0/16, 2.0/16, 2.0/16
  	);
	//Edge-detection
	float edge_detection_kernel[9] = float[](
		1,  1,  1,
		1, -8,  1,
		1,  1,  1
	);

	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++){
		sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; i++)
		col += sampleTex[i] * normal_kernel[i];		// choose a kernel here (blur, sharpen...)
	FragColor = vec4(col, 1.0);

	//-------------Weighted Grey-------------
//	vec4 col = texture(screenTexture, TexCoords);
//	float avg = col.r*0.2 + col.g*0.7 + col.b*0.07;
//	FragColor = vec4(avg, avg, avg, 1.0);


	//-------------Inverse-------------
//	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

	//-------------Original-------------
//	FragColor = vec4(vec3(texture(screenTexture, TexCoords)), 1.0);
}