#version 330 core
in vec4 ex_Color;
in vec3 ex_Normal;
in vec3 ex_Pos;
in vec2 ex_Texcoord;

layout(location = 0) out vec4 FragColor;
//uniform vec4 Color;

uniform samplerCube ourTexture;

uniform vec3 viewPos;


void main(void)
{
	vec3 N = normalize(ex_Normal);

	vec3 I = normalize(viewPos - ex_Pos);
	vec3 R = reflect(-I, N); 
	vec3 Re = refract(-I,N,0.95);
	//Re = refract(Re,N,1.5168/1.00027717);

	float f = pow((1.00027717 - 1.5168),2)/(1.00027717 + 1.5168);

	float F = f + (1 - f) * pow((1 - max(dot(I, N), 0.0)),5);
                  
	vec3 ReflectColor = vec3(texture(ourTexture, R));
	vec3 RefractColor = vec3(texture(ourTexture, Re));

	vec3 c = mix(RefractColor, ReflectColor, F);
	float luma = 0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z;
	FragColor = vec4(c, F*0.7 + 0.3);

	//FragColor = ex_Color;
}