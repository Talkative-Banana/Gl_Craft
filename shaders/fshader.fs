#version 330 core

in vec3 TexCoord;
out vec4 outColor;

uniform samplerCube cubemap;

void main() {
        //outColor = vec4(TexCoord, 1.0);
        outColor = texture(cubemap, TexCoord);
}
