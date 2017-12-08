#version 330 core

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec4 vColor;
smooth out vec4 vSmoothColor;
uniform mat4 MVP;
uniform int tesMult;

void main()
{
	vSmoothColor = vec4(vColor);
	float z = tesMult*(vColor.r + vColor.g + vColor.b)/3;
    gl_Position = MVP*vec4(vVertex.x, vVertex.y, z, 1);
}
