#version 450

layout(location = 0) out vec4 outColor;

layout(location = 3) in vec3 normal;

void main() {
    //outColor = vec4(1.0, 0.0, 1.0, 1.0);
    outColor = vec4(normal, 1.0);
}
