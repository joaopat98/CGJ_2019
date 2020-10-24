#version 330 core
in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec3 ex_Color;
out vec3 ex_Normal;
out vec3 ex_Pos;
out vec2 ex_Texcoord;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform vec3 Color;




void main(void)
{
	ex_Pos = inPosition;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition,1.0);
	ex_Color = Color;
	ex_Normal = inNormal;
	ex_Texcoord = inTexCoord;
}