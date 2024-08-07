#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

layout(location = 0) in vec3 pos;

layout(location = 0) out vec3 dir;

void main() {
   gl_Position = vec4(pos, 1.0);
   vec4 tmp = ubo.mvp * vec4(pos, 1.0);
   dir = tmp.xyz;
}
