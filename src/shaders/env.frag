#version 450

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 dir;

layout(binding = 1) uniform samplerCube env;

void main() {
    color = texture(env, dir);
}
