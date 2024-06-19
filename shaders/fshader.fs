#version 330 core

in vec4 fColor;
out vec4 outColor;
void main(void) {
        //outColor = vec4(fColor, 1.0);
        outColor = fColor;
}
