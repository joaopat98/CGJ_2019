#version 330 core

in vec4 exColor;
in vec3 exNormal;
in vec2 exTexCoord;
layout(location = 0) out vec4 out_Color;

void main(void)
{
	vec2 dummy = exTexCoord;
	float intensity = max(dot(normalize(exNormal),normalize(vec3(0.6,0.7,0.8))),0);
	//out_Color = vec4(exNormal,1);
	out_Color = exColor * intensity;
}