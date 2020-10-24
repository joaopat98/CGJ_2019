#version 330 core
in vec3 inPosition;
//in vec4 in_Color;
in vec3 inNormal;
in vec2 inTexCoord;

out vec4 ex_Color;
out vec3 ex_Normal;
out vec3 ex_Pos;
out vec2 ex_Texcoord;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform vec4 Color;

uniform vec3 viewPos;


void main(void)
{
	ex_Pos = vec3(ModelMatrix * vec4(inPosition, 1));
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition,1.0);
	ex_Color = Color;
	ex_Normal = mat3(transpose(inverse(ModelMatrix))) * inNormal;
	ex_Texcoord = inTexCoord;
}