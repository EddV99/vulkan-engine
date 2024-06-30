#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 viewDirection;

layout(location = 0) out vec4 outColor;

vec3 blinn();

void main() {
    // vec4 tColor = texture(texSampler, texCoord);
    //outColor = vec4(normalize(normal), 1.0);
    outColor = vec4(blinn(), 1.0);
}

vec3 blinn() {
    vec3 n = normalize(normal);

    vec3 I = vec3(1.0, 1.0, 1.0); // Light color
    vec3 I_a = vec3(0.3, 0.3, 0.3); // Ambient light color

    vec3 K_d = vec3(1.0, 0.0, 0.0); // Material diffuse color
    vec3 K_s = vec3(1.0, 1.0, 1.0); // Specular color
    vec3 K_a = K_d;

    vec3 lightDirection = vec3(1.0, 0.0, 0.0);

    float alpha = 1000.0;

    vec3 h = normalize(lightDirection + normalize(viewDirection));

    float cosTheta = dot(lightDirection, n);
    float cosPhi = dot(n, h);

    vec3 color = I * (max(0.0, cosTheta) * K_d + K_s * pow(max(0.0, cosPhi), alpha)) + I_a * K_a;
    return color;
}
