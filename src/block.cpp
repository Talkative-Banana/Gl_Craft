#include "block.h"

#include "Renderer.h"

block::block(const glm::ivec3& pos, GLboolean solid) {
  position = pos;
  is_solid = solid;
}

block::block() : position(0), is_solid(false) {
}

GLuint block::Mask(GLuint X, GLuint Y, GLuint Z, GLuint cent, GLuint normal) {
  GLuint mask = 0;
  // tttttttnnnccczzzzzzyyyyyyxxxxxx 7 all add
  mask |= X | (Y << 6) | (Z << 12) | (cent << 18) | (normal << 21);
  return mask;
}

std::vector<GLuint> block::GenerateVerticies() {
  GLuint x = position.x, y = position.y, z = position.z;
  // Vertex Position
  // 24 verticies per block

  std::vector<GLuint> vertices;

  // Back Face 0123
  vertices.push_back(Mask(x, y, z, 7, 0));  // 0
  // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 0
  vertices.push_back(Mask(x, y + 1, z, 5, 0));  // 1
  // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1 1
  vertices.push_back(Mask(x + 1, y + 1, z, 1, 0));  // 2
  // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2 2
  vertices.push_back(Mask(x + 1, y, z, 3, 0));  // 3
  // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3 3

  // Front Face 4567
  vertices.push_back(Mask(x, y, z + 1, 6, 1));  // 4
  // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4 4
  vertices.push_back(Mask(x, y + 1, z + 1, 4, 1));  // 5
  // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5 5
  vertices.push_back(Mask(x + 1, y + 1, z + 1, 0, 1));  // 6
  // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6 6
  vertices.push_back(Mask(x + 1, y, z + 1, 2, 1));  // 7
  // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7 7

  // Left Face 0154
  vertices.push_back(Mask(x, y, z, 7, 2));  // 0
  // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 8
  vertices.push_back(Mask(x, y + 1, z, 5, 2));  // 1
  // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1 9
  vertices.push_back(Mask(x, y + 1, z + 1, 4, 2));  // 5
  // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5 10
  vertices.push_back(Mask(x, y, z + 1, 6, 2));  // 4
  // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4 11

  // Right Face 7623
  vertices.push_back(Mask(x + 1, y, z + 1, 2, 3));  // 7
  // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7 12
  vertices.push_back(Mask(x + 1, y + 1, z + 1, 0, 3));  // 6
  // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6 13
  vertices.push_back(Mask(x + 1, y + 1, z, 1, 3));  // 2
  // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2 14
  vertices.push_back(Mask(x + 1, y, z, 3, 3));  // 3
  // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3 15

  // Top Face 5126
  vertices.push_back(Mask(x, y + 1, z + 1, 4, 4));  // 5
  // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5 16
  vertices.push_back(Mask(x, y + 1, z, 5, 4));  // 1
  // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1 17
  vertices.push_back(Mask(x + 1, y + 1, z, 1, 4));  // 2
  // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2 18
  vertices.push_back(Mask(x + 1, y + 1, z + 1, 0, 4));  // 6
  // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6 19

  // Bottom Face 0473
  vertices.push_back(Mask(x, y, z, 7, 5));  // 0
  // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 20
  vertices.push_back(Mask(x, y, z + 1, 6, 5));  // 4
  // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4 21
  vertices.push_back(Mask(x + 1, y, z + 1, 2, 5));  // 7
  // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7 22
  vertices.push_back(Mask(x + 1, y, z, 3, 5));  // 3
  // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3 23

  return vertices;
}

void block::Render(GLuint mask, std::vector<GLuint>& indices, std::vector<GLuint>& rendervert) {
  if (!is_solid) return;
  rendervert = GenerateVerticies();

  GLuint idx = 0;
  while (mask != 0) {
    if (mask & 1) {
      for (int i = 0; i < 6; i++) indices.push_back(faceindices[idx][i]);
    }
    mask >>= 1, idx++;
  }
}
