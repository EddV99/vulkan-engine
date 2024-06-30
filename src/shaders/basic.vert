#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat3 mvn;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 texCoord;
layout(location = 2) out vec3 viewDirection;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
    normal = ubo.mvn * nor;
    texCoord = uv;

    vec4 mvPos = ubo.view * ubo.model * vec4(pos, 1.0);
    viewDirection = -1.0 * mvPos.xyz;
}
