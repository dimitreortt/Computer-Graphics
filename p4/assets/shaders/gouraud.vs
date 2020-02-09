#version 330 core

struct Light
{
	int type;			// 
	int falloff;		// 
	vec4 color;			// 
	vec3 position;		// 
	vec3 direction;		// direcao da luz spot e luz direcional
	float ghama;		// notacao do capitulo 4 para luz spot
	float fallExponent;	// el, da luz spot
};

struct Material
{
	vec4 Oa;	// 
	vec4 Od;	// 
	vec4 Os;	// 
	float Ns;	// 
};

uniform mat4 transform;
uniform mat3 normalMatrix;
uniform mat4 vpMatrix = mat4(1);

uniform vec3 cameraPosition;
uniform vec4 ambientLight;

uniform int NL;				// number of lights (max 10)
uniform Light lights[10];	// vector of lights
uniform Material m;			// material of mesh

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 vertexColor;

void main()
{
	vec3 Ll;	// 
	vec3 Rl;	// 
	vec3 V;		// 
	vec4 Il;	// light intensity in point
	float Dl;	// distance between point and light

	vec4 P = transform * position;
	vec3 N = normalize(normalMatrix * normal); 
	gl_Position = vpMatrix * P;

	// initialization of temporary color of vertex
	vec4 temporaryColor = vec4(0);

	for(int i = 0; i < NL; i++)
	{
		if(lights[i].type == 0) // Directional light
		{
			Ll = normalize(lights[i].direction);
			Il = lights[i].color;
		}

		else if(lights[i].type == 1)	// Point light
		{
			Dl = distance(lights[i].position, vec3(P));
			Ll = (vec3(P) - lights[i].position)/Dl;
			Il = lights[i].color / pow(Dl,lights[i].falloff);
		}

		else if(lights[i].type == 2)	// Spot light
		{
			Dl = distance(lights[i].position, vec3(P));
			Ll = (vec3(P) - lights[i].position)/Dl;

			// capitulo 4 sobre luz spot
			vec3 dir = normalize(lights[i].direction);
			float fi = acos(dot(dir, Ll));

			if (fi <= lights[i].ghama)	// se o ponto esta no alcance da luz spot
			{
				Il = (lights[i].color / pow(Dl,lights[i].falloff)) * pow(cos(fi),lights[i].fallExponent);
			}
			else
			{
				Il = vec4(0.0);
			}			
		}
		
		V = normalize(vec3(P) - cameraPosition);

		Rl = Ll - 2 * dot(N, Ll) * N;

		temporaryColor += + m.Od * Il * max(dot(-N, Ll), 0) + m.Os * Il * pow(max(dot(-Rl, V), 0), m.Ns);
	}	

	// luz ambiente
	temporaryColor += m.Oa * ambientLight;

	// atribui cor calculada a cor do vertice
	vertexColor = temporaryColor;
}