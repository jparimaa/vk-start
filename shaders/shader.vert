#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform TransformationMatrices
{
    mat4 world;
    mat4 view;
    mat4 proj;
}
ubo;

layout(location = 0) in vec3 inPosition;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.world * vec4(inPosition, 1.0);
}