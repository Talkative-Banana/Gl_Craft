#include <array>

#include "Constants.hpp"
#include "Utils.h"
#pragma once

class Block {
 private:
  std::vector<GLuint> GenerateVerticies(GLuint ambient_occ);

 public:
  // visible solid
  // 00000000_00000000_00000000_00000000
  // ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^
  // |||||||| |||||||| |||||||| ||||||||
  // ooootttt tmmmmmmv sxxxxxyy yyyzzzzz
  //             rlfb
  uint32_t blmask;
  static constexpr std::array<std::array<unsigned int, 6>, 6> faceindices = {
      {{{2, 3, 0, 2, 0, 1}},          // Back
       {{7, 6, 5, 7, 5, 4}},          // Front
       {{11, 10, 9, 11, 9, 8}},       // Left
       {{15, 14, 13, 15, 13, 12}},    // Right
       {{19, 18, 17, 19, 17, 16}},    // Top
       {{23, 22, 21, 23, 21, 20}}}};  // Bottom


  Block(const glm::ivec3& pos, GLboolean solid, GLuint bltype);
  Block();

  void Render(
      GLuint mask,
      GLuint ambient_occ,
      std::vector<GLuint>& indices,
      std::vector<GLuint>& rendervert);
  void add(int bltype);
  void remove();
  bool isSolid();
  bool is_transparent(GLuint blkmask);
  GLuint Mask(GLuint X, GLuint Y, GLuint Z, GLuint cent, GLuint normal, GLuint blktype, GLuint ac);
};
