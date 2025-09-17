#include "Block.h"

#include "Renderer.h"

Block::Block(const glm::ivec3& pos, GLboolean solid, GLuint bltype) {
  blmask = (bltype << 23) | (pos.x << 10) | (pos.y << 5) | (pos.z);
  if (solid) blmask |= (1 << 15);
}

Block::Block() : blmask(0) {
}

GLuint Block::Mask(GLuint X, GLuint Y, GLuint Z, GLuint cent, GLuint normal, GLuint bltype) {
  GLuint mask = 0;
  // tttttttnnnccczzzzzzyyyyyyxxxxxx 7 all add [block can have their final point at 32]
  mask |= X | (Y << 6) | (Z << 12) | (cent << 18) | (normal << 21) | (bltype << 24);
  return mask;
}

std::vector<GLuint> Block::GenerateVerticies() {
  GLuint x = (blmask >> 10) & 31, y = (blmask >> 5) & 31, z = (blmask) & 31,
         blktype = (blmask >> 23) & 63;
  // Vertex Position
  // 24 verticies per block

  std::vector<GLuint> vertices;

  // Back Face 0123
  vertices.push_back(Mask(x, y, z, 7, 0, blktype));  // 0
  // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 0
  vertices.push_back(Mask(x, y + 1, z, 5, 0, blktype));  // 1
  // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1 1
  vertices.push_back(Mask(x + 1, y + 1, z, 1, 0, blktype));  // 2
  // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2 2
  vertices.push_back(Mask(x + 1, y, z, 3, 0, blktype));  // 3
  // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3 3

  // Front Face 4567
  vertices.push_back(Mask(x, y, z + 1, 6, 1, blktype));  // 4
  // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4 4
  vertices.push_back(Mask(x, y + 1, z + 1, 4, 1, blktype));  // 5
  // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5 5
  vertices.push_back(Mask(x + 1, y + 1, z + 1, 0, 1, blktype));  // 6
  // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6 6
  vertices.push_back(Mask(x + 1, y, z + 1, 2, 1, blktype));  // 7
  // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7 7

  // Left Face 0154
  vertices.push_back(Mask(x, y, z, 7, 2, blktype));  // 0
  // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 8
  vertices.push_back(Mask(x, y + 1, z, 5, 2, blktype));  // 1
  // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1 9
  vertices.push_back(Mask(x, y + 1, z + 1, 4, 2, blktype));  // 5
  // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5 10
  vertices.push_back(Mask(x, y, z + 1, 6, 2, blktype));  // 4
  // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4 11

  // Right Face 7623
  vertices.push_back(Mask(x + 1, y, z + 1, 2, 3, blktype));  // 7
  // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7 12
  vertices.push_back(Mask(x + 1, y + 1, z + 1, 0, 3, blktype));  // 6
  // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6 13
  vertices.push_back(Mask(x + 1, y + 1, z, 1, 3, blktype));  // 2
  // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2 14
  vertices.push_back(Mask(x + 1, y, z, 3, 3, blktype));  // 3
  // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3 15

  // Top Face 5126
  vertices.push_back(Mask(x, y + 1, z + 1, 4, 4, blktype));  // 5
  // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5 16
  vertices.push_back(Mask(x, y + 1, z, 5, 4, blktype));  // 1
  // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1 17
  vertices.push_back(Mask(x + 1, y + 1, z, 1, 4, blktype));  // 2
  // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2 18
  vertices.push_back(Mask(x + 1, y + 1, z + 1, 0, 4, blktype));  // 6
  // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6 19

  // Bottom Face 0473
  vertices.push_back(Mask(x, y, z, 7, 5, blktype));  // 0
  // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 20
  vertices.push_back(Mask(x, y, z + 1, 6, 5, blktype));  // 4
  // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4 21
  vertices.push_back(Mask(x + 1, y, z + 1, 2, 5, blktype));  // 7
  // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7 22
  vertices.push_back(Mask(x + 1, y, z, 3, 5, blktype));  // 3
  // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3 23

  return vertices;
}

void Block::Render(GLuint mask, std::vector<GLuint>& indices, std::vector<GLuint>& rendervert) {
  if (((blmask >> 15) & 1) == 0) return;  // not solid
  rendervert = GenerateVerticies();

  GLuint idx = 0;
  while (mask != 0) {
    blmask |= (1 << 16);  // mark them visible if any side is visble
    if (mask & 1) {
      for (int i = 0; i < 6; i++) indices.push_back(faceindices[idx][i]);
    }
    mask >>= 1, idx++;
  }
}

void Block::remove() {
  blmask &= ~(1 << 15);  // clear solid bit
  blmask &= ~(1 << 16);  // clear visible bit
}

void Block::add() {
  blmask |= (3 << 15);  // add solid and visble bit
}
