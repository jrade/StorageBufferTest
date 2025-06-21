#version 440 core

layout (std140, binding = 0) uniform UniformBlock
{
    mat4 mvpMatrix;
};

layout (std430, binding = 1) buffer ColorBuffer
{
    vec4 vertColor[];
};

layout(location = 0) in vec2 vertPos;
layout(location = 0) out vec4 fragColor;

void main()
{
    gl_Position = mvpMatrix * vec4(vertPos, 0.0, 1.0);
    fragColor = vertColor[gl_VertexIndex / 4];
}
