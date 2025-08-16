#include <noise/noise.h>
#include <noiseutils.h>

#include <memory>

#include "block.h"
#include "constants.hpp"
#include "utils.h"

#pragma once

class chunk {
 private:
  GLboolean displaychunk;
  glm::ivec3 chunkpos;
  block blocks[CHUNK_BLOCK_COUNT][CHUNK_BLOCK_COUNT][CHUNK_BLOCK_COUNT];

 public:
  GLuint id;
  GLuint count;
  std::vector<std::pair<std::vector<GLuint>, std::vector<GLuint>>> rendervert;

  chunk(uint _id, glm::ivec3 position, GLboolean display);

  void Render();
  void Setup_Landscape(GLint X, GLint Y);
  GLuint RenderFace(std::vector<GLint> &&position);
  inline GLboolean isSolid(const std::vector<GLint> &postion);
};
