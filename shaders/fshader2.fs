#version 330 core

out vec4 outColor;

vec3 fColor = vec3(1.0, 1.0, 1.0);

void main() {
        // outColor = vec4(NormalDir, 1.0);
        outColor = vec4(fColor, 1.0);
}
