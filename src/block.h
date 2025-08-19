#include <array>

#include "utils.h"
#pragma once

class block {
 private:
  std::vector<GLuint> GenerateVerticies();

 public:
  glm::ivec3 position;
  bool is_solid;
  static constexpr std::array<std::array<unsigned int, 6>, 6> faceindices = {
      {{{2, 3, 0, 2, 0, 1}},          // Back
       {{7, 6, 5, 7, 5, 4}},          // Front
       {{11, 10, 9, 11, 9, 8}},       // Left
       {{15, 14, 13, 15, 13, 12}},    // Right
       {{19, 18, 17, 19, 17, 16}},    // Top
       {{23, 22, 21, 23, 21, 20}}}};  // Bottom


  block(const glm::ivec3& pos, GLboolean solid);
  block();

  void Render(GLuint mask, std::vector<GLuint>& indices, std::vector<GLuint>& rendervert);
  GLuint Mask(GLuint X, GLuint Y, GLuint Z, GLuint cent, GLuint normal);
};
