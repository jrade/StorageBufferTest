#version 440 core

layout(location = 0) in vec4 fragColor;
layout(location = 0) out vec4 fragColor1;

void main()
{
    fragColor1 = fragColor;
}
