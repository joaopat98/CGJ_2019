#version 330 core

in vec4 exColor;
in vec3 exNormal;
in vec2 exTexCoord;
in vec3 exPosition;
out vec4 out_Color;

uniform vec3 u_resolution;

uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 specularLight;
uniform vec3 lightPos;
uniform vec3 viewPos;

float hash(float x, float y) 
{
    return fract(abs(sin(sin(123.321 + x) * (y + 321.123)) * 456.654));
}
float lerp(float a, float b, float t) 
{
    return a * (1.0 - t) + b * t;
}
float perlin(float x, float y, float z) 
{
    float col = 0.0;
    for (int i = 0; i < 8; i++) 
    {
        float fx = floor(x);
        float fy = floor(y);
        float fz = floor(z);
        float cx = ceil(x);
        float cy = ceil(y);
        float cz = ceil(z);
        float a = hash(fx, fy);
        float b = hash(fx, cy);
        float c = hash(cx, fy);
        float d = hash(cx, cy);
        float e = hash(fx, fz);
        float f = hash(fx, cz);
        float g = hash(cx, fz);
        float h = hash(cx, cz);
        col += lerp(lerp(lerp(a, b, fract(y)), lerp(c, d, fract(y)), fract(x)), lerp(lerp(e, f, fract(z)), lerp(g, h, fract(z)), fract(x)), fract(0.5));
        col /= 2.0;
        x /= 2.0;
        y /= 2.0;
        z /= 2.0;
    }
    return col;
}
float dperlin(float x, float y, float z) 
{
    float d = perlin(x, y, z) * 800.0;
    return perlin(x + d, y + d, z + d);
}
float ddperlin(float x, float y, float z) 
{
    float d = perlin(x, y, z) * 800.0;
    return dperlin(x + d, y + d, z + d);
}
void main(void) 
{
    vec3 p = (exPosition.xyz / u_resolution.xyz);
    p *= 800.0;
    float d = perlin(p.x, p.y, p.z) * 800.0 + sin(1.0 / 800.0) * 800.;
    float u = p.x + d;
    float v = p.y + d;
    float w = p.z + d;
    d = perlin(u, v, w) * 800.0;
    float color = perlin(p.x + d, p.y + d, p.z + d);
    out_Color = vec4(vec3(2.0 * color) - vec3(color * .1, color * .2 - sin(u / 30.0) * .1, color * .3 + sin(v / 40.0) * .2), 1.0);

	vec3 N = normalize(exNormal);
	vec3 L = normalize(lightPos - exPosition); 

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = diff * diffuseLight;

	vec3 E = normalize(viewPos - exPosition);
	vec3 R = reflect(-L, N);

	float spec = pow(max(dot(E, R), 0.0), 32);
	vec3 specular = specularLight * spec;


	vec3 l = ambientLight + diffuse + specular;

	out_Color = vec4(l.xyz * out_Color.xyz, 1);
}
