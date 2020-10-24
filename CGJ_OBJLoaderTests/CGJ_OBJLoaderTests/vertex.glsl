#version 330 core
in vec3 inPosition;
in vec2 inTexcoord;
in vec3 inNormal;

out vec4 exColor;
out vec3 exNormal;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 Color;

void main(void)
{
	mat4 oof = ProjectionMatrix;
	oof = ViewMatrix;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition,1);
	exColor = Color;
	//exNormal = vec3(transpose(inverse(ViewMatrix)) * vec4(normalize(inNormal),1));
	exNormal = inNormal;
}