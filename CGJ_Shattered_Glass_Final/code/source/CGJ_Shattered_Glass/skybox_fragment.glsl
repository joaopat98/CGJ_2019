#version 330 core
in vec3 ex_Color;
in vec3 ex_Normal;
in vec3 ex_Pos;
in vec2 ex_Texcoord;

layout(location = 0) out vec4 FragColor;
//uniform vec4 Color;

uniform samplerCube skybox;


void main(void)
{
	
	FragColor = texture(skybox, ex_Pos);
}