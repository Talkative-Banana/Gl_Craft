#include "chunk.h"

#include <cstdlib>
#include <string>

#include "Renderer.h"

chunk::chunk(uint _id, glm::ivec3 _biomepos, glm::ivec3 position, GLboolean display) {
  id = _id;
  count = 0;
  biomepos = _biomepos;
  chunkpos = position;
  displaychunk = display;
}

inline GLboolean chunk::isSolid(const std::vector<GLint> &position) {
  if ((position[0] >= 0) && (position[0] < CHUNK_BLOCK_COUNT) && (position[1] >= 0) &&
      (position[1] < CHUNK_BLOCK_COUNT) && (position[2] >= 0) &&
      (position[2] < CHUNK_BLOCK_COUNT)) {
    return blocks[position[0]][position[1]][position[2]].is_solid;
  }
  return false;
}

// k blue i red j green
// ctrl x -> red facing me

GLuint chunk::RenderFace(std::vector<GLint> &&position) {
  // 1 -> back face: 2 -> front face: 3 -> left face: 4 -> right face: 5 -> top face: 6 -> bottom
  // face
  GLuint mask = 0;
  std::vector<GLint> tmp = position;
  for (GLuint face = 1; face <= 6; face++) {
    if (face == 1) {
      // No Need to draw back face if block behind is solid
      tmp[2] -= 1;
      if (!isSolid(tmp)) mask |= (1 << (face - 1));
      tmp[2] += 1;
    } else if (face == 2) {
      // No Need to draw front face if block in front is solid
      tmp[2] += 1;
      if (!isSolid(tmp)) mask |= (1 << (face - 1));
      tmp[2] -= 1;
    } else if (face == 3) {
      // No Need to draw left face if block in left is solid
      tmp[0] -= 1;
      if (!isSolid(tmp)) mask |= (1 << (face - 1));
      tmp[0] += 1;
    } else if (face == 4) {
      // No Need to draw right face if block in right is solid
      tmp[0] += 1;
      if (!isSolid(tmp)) mask |= (1 << (face - 1));
      tmp[0] -= 1;
    } else if (face == 5) {
      // No Need to draw top face if block on top is solid
      tmp[1] += 1;
      if (!isSolid(tmp)) mask |= (1 << (face - 1));
      tmp[1] -= 1;
    } else if (face == 6) {
      // No Need to draw bottom face if block on bottom is solid
      tmp[1] -= 1;
      if (!isSolid(tmp)) mask |= (1 << (face - 1));
      tmp[1] += 1;
    }
  }
  return mask;
}

void chunk::Setup_Landscape(GLint X, GLint Z) {

  // 4 5 6 7
  // 0 1 2 3
  noise::module::RidgedMulti mountainTerrain;
  noise::module::Billow baseFlatTerrain;
  baseFlatTerrain.SetFrequency(2.0);

  noise::module::ScaleBias flatTerrain;
  flatTerrain.SetSourceModule(0, baseFlatTerrain);
  flatTerrain.SetScale(0.085);

  noise::utils::NoiseMap heightMap;
  noise::utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule(flatTerrain);
  heightMapBuilder.SetDestNoiseMap(heightMap);
  heightMapBuilder.SetDestSize(256, 256);
  heightMapBuilder.SetBounds(4.0 * Z, 4.0 * (Z + 1), 4.0 * X, 4.0 * (X + 1));
  heightMapBuilder.Build();

  noise::utils::RendererImage renderer;
  noise::utils::Image image;
  renderer.SetSourceNoiseMap(heightMap);
  renderer.SetDestImage(image);
  renderer.Render();

  noise::utils::WriterBMP writer;
  writer.SetSourceImage(image);
  writer.SetDestFilename("maps/tutorial" + std::to_string(4 * X + Z) + ".bmp");
  writer.WriteDestFile();

  for (int x = 0; x < CHUNK_BLOCK_COUNT; x++) {
    for (int z = 0; z < CHUNK_BLOCK_COUNT; z++) {
      // Use the noise library to get the height value of x, z
      noise::utils::Color color = image.GetValue(x, z);
      // Extract the height value from the color's red channel (assuming height is encoded in the
      // red channel)
      int height = std::max(1, static_cast<int>((color.blue / 255.0f) * 32.0f));
      // Use the height map texture to get the height value of x, z
      for (int y = 0; y < height; y++) {
        blocks[z][y][x].is_solid = 1;
      }
    }
  }
}

void chunk::Render() {
  if (!displaychunk) return;
  GLuint idx = 0;

  Setup_Landscape(chunkpos.x, chunkpos.z);

  for (int i = 0; i < CHUNK_BLOCK_COUNT; i++) {
    for (int k = 0; k < CHUNK_BLOCK_COUNT; k++) {
      for (int j = 0; j < CHUNK_BLOCK_COUNT; j++) {
        // filled[0][0][0] = 1;
        if (!blocks[i][j][k].is_solid) break;
        glm::ivec3 ofs = {i, j, k};
        GLuint mask = chunk::RenderFace({i, j, k});
        blocks[i][j][k] = block(biomepos + ofs, true);
        std::vector<GLuint> indices;
        std::vector<GLuint> blockrendervert;
        blocks[i][j][k].Render(mask, indices, blockrendervert);
        for (auto &ind : indices) ind += idx;
        rendervert.push_back({blockrendervert, indices});
        idx += 24, count++;
      }
    }
  }
}
