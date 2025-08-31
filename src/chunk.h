#include <noise/noise.h>
#include <noiseutils.h>

#include <memory>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "block.h"
#include "constants.hpp"
#include "utils.h"

#pragma once

class chunk {
 private:
  GLboolean displaychunk;
  glm::ivec3 biomepos;
  glm::ivec3 chunkpos;

 public:
  block blocks[CHUNK_BLOCK_COUNT][CHUNK_BLOCK_COUNT][CHUNK_BLOCK_COUNT];
  std::vector<GLuint> cube_vertices;
  std::vector<GLuint> cube_indices;

 public:
  GLuint id;
  GLuint count;
  std::vector<std::pair<std::vector<GLuint>, std::vector<GLuint>>> rendervert;

  chunk(uint _id, glm::ivec3 biomepos, glm::ivec3 position, GLboolean display);

  void Render(
      std::vector<std::unique_ptr<VertexArray>> &chunkva,
      std::vector<unsigned int> &cntblocks);
  void Setup_Landscape(GLint X, GLint Y);
  GLuint RenderFace(std::vector<GLint> &&position);
  inline GLboolean isSolid(const std::vector<GLint> &postion);
};
