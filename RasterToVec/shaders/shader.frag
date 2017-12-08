#version 330 core

layout(location = 0) out vec4 vFragColor;
smooth in vec4 vSmoothColor;

void main()
{
    vFragColor = vSmoothColor;
}
