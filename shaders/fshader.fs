#version 330 core

in vec3 TexCoord;
in vec3 NormalDir;
in float isoscale;
out vec4 outColor;

uniform samplerCube cubemap;

void main() {
        // outColor = vec4(NormalDir, 1.0);
        outColor = isoscale * texture(cubemap, TexCoord);
}
