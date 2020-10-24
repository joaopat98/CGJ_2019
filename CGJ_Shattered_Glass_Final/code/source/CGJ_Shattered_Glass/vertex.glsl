#version 330 core
in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

out vec4 exColor;
out vec3 exNormal;
out vec2 exTexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 Color;

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition,1);
	exColor = Color;
	exNormal = vec3(transpose(inverse(ModelMatrix)) * vec4(inNormal,0));
}