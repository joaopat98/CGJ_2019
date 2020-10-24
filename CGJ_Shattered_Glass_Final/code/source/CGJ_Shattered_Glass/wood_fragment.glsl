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


float lines(in vec3 pos, float b){
    float scale = 25.0;
    pos *= scale;
    return smoothstep(0.0, .5+b*.5, abs((sin(pos.x*3.1415)+b*2))*.5);
}

/////

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v) {
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
                    i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
            + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    // Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                dot(p2,x2), dot(p3,x3) ) );
}

/////

float random (in vec3 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main(void)
{
	vec2 dummy = exTexCoord;
	float intensity = max(dot(normalize(exNormal),normalize(vec3(0.6,0.7,0.8))),0);
	//out_Color = vec4(exNormal,1);
	//out_Color = exColor * intensity;



	//vec2 st = exTexCoord.xy / u_resolution.xy;

    // Scale the coordinate system to see
    // some noise in action
    //vec2 pos = vec2(exTexCoord*5.0);

    // Use the noise function
    //float n = noise(vec2(noise(pos + vec2(263,827)),noise(pos + vec2(8733,1992))));

    //out_Color = vec4(vec3(n), 1.0);

	/////////////


	vec3 st = exPosition.xyz / u_resolution.xyz;
	st -= vec3(0.5, 0.5 ,0.5);

    // Add noise
	float val = (snoise(st) + 1)/2; 
    vec3 pos = vec3(val);

    // Draw lines
	float distortion = snoise(pos * 200) * 0.4;
    float pattern = min(1,pow(abs(lines(pos, 0.1)),2)) + distortion;

	
	vec4 color1 = vec4(0.1843137254901961, 0.08627450980392157, 0.023529411764705882, 1);
	vec4 color2 = vec4(0.21568627450980393, 0.10980392156862745, 0.03529411764705882, 1);

    out_Color = mix(color2,color1,pattern);

	vec3 N = normalize(exNormal);
	vec3 L = normalize(lightPos - exPosition); 

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = diff * diffuseLight;

	float specularStrength = 1;
	vec3 E = normalize(viewPos - exPosition);
	vec3 R = reflect(-L, N);

	float spec = pow(max(dot(E, R), 0.0), 32);
	vec3 specular = (1-pattern) * specularStrength * specularLight * spec; //F * spec ; 

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * ambientLight;

	vec4 ambient4 = vec4(ambient, 1.0);
	vec4 diffuse4 = vec4(diffuse, 1.0);
	vec4 specular4 = vec4(specular, 1.0);

	vec4 result = (ambient4 + diffuse4 + specular4) * out_Color;

	out_Color = result;
}