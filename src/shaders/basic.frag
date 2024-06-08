#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(1.0, 0.0, 1.0, 1.0);
    outColor = vec4(normal, 1.0);
    vec4 tColor = texture(texSampler, texCoord);
}
