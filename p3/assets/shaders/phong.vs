#version 330 core

uniform mat4 transform;
uniform mat3 normalMatrix;
uniform mat4 vpMatrix = mat4(1);

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 P;
out vec3 N;

void main()
{
	P = transform * position;
	N = normalize(normalMatrix * normal); 
	gl_Position = vpMatrix * P;
}
