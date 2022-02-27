#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(binding = 0) uniform UBO
{
    mat4 viewProjection;
}
ubo;

layout(location = 0) out vec4 outColor;

void main()
{
    gl_Position = ubo.viewProjection * vec4(inPosition, 1.0);
    outColor = vec4(0.3, 0.5, 0.7, 1.0);
}
