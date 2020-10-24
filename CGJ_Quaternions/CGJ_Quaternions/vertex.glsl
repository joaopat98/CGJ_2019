#version 330 core

in vec4 in_Position;
out vec4 ex_Color;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 Color;

void main(void)
{
	mat4 oof = ProjectionMatrix;
	oof = ViewMatrix;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * in_Position;
	ex_Color = Color;
}