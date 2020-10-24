#version 330 core
in vec3 ex_Color;
in vec3 ex_Normal;
in vec3 ex_Pos;
in vec2 ex_Texcoord;

layout(location = 0) out vec4 FragColor;
//uniform vec4 Color;

uniform sampler2D tex;


void main(void)
{
	float intensity = max(dot(normalize(ex_Normal),normalize(vec3(0.6,0.7,0.8))),0) * 0.8 + 0.2;
	//out_Color = vec4(exNormal,1);
	FragColor = texture(tex, ex_Texcoord) * intensity;
}