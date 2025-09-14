#include <noise/noise.h>
#include <noiseutils.h>

#include <memory>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "block.h"
#include "constants.hpp"
#include "utils.h"

#pragma once

extern GLint chunkpos_uniform;
extern GLuint wireframemode;

class chunk {
 public:
  glm::ivec3 biomepos;
  glm::ivec3 chunkpos;

  GLboolean displaychunk;
  std::array<std::array<std::array<block, CHUNK_BLOCK_COUNT>, CHUNK_BLOCK_COUNT>, CHUNK_BLOCK_COUNT>
      blocks;
  std::vector<GLuint> cube_vertices;
  std::vector<GLuint> cube_indices;
  std::unique_ptr<VertexArray> chunkva;
  GLuint cntblocks;


 public:
  GLuint id;
  GLuint count;
  std::vector<std::pair<std::vector<GLuint>, std::vector<GLuint>>> rendervert;

  chunk(uint _id, glm::ivec3 biomepos, glm::ivec3 position, GLboolean display);

  void Render(
      int setup,
      bool firstRun,
      std::shared_ptr<chunk>,   // left
      std::shared_ptr<chunk>,   // forward
      std::shared_ptr<chunk>,   // right
      std::shared_ptr<chunk>);  // back
  void Setup_Landscape(GLint X, GLint Y);
  GLuint RenderFace(std::vector<GLint> &&position);
  inline GLboolean isSolid(const std::vector<GLint> &postion);
  void Draw();
};
