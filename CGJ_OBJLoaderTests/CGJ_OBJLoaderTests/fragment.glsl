#version 330 core

in vec4 exColor;
in vec3 exNormal;
out vec4 out_Color;

void main(void)
{
	float intensity = max(dot(exNormal,vec3(1,0,0)),0);
	out_Color = vec4(exNormal,1);//vec4(intensity,0,0,1);

}