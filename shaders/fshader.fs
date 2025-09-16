#version 330 core

in vec2 tile;
in vec3 NormalDir;
in float isoscale;
in vec2 TexCoord;
out vec4 outColor;

uniform sampler2D atlas;

void main() {
   int atlasSize = 16;                // 16x16 grid
   vec2 tileSize = 1.0 / vec2(atlasSize, atlasSize);
   vec2 atlasUV = TexCoord * tileSize + vec2(tile.x, tile.y) * tileSize;

   outColor = isoscale * texture(atlas, atlasUV);
}
