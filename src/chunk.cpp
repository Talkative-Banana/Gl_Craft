#include "chunk.h"

#include <cstdlib>
#include <random>
#include <string>

#include "Renderer.h"

chunk::chunk(uint _id, glm::ivec3 _biomepos, glm::ivec3 position, GLboolean display) {
  id = _id;
  count = 0;
  biomepos = _biomepos;
  chunkpos = position;
  displaychunk = display;
  Setup_Landscape(chunkpos.x, chunkpos.z);
}

inline GLboolean chunk::isSolid(const std::vector<GLint> &position) {
  if ((position[0] >= 0) && (position[0] < CHUNK_BLOCK_COUNT) && (position[1] >= 0) &&
      (position[1] < CHUNK_BLOCK_COUNT) && (position[2] >= 0) &&
      (position[2] < CHUNK_BLOCK_COUNT)) {

    // Check if neibhourung block is solid
    return ((blocks[position[0]][position[1]][position[2]].blmask >> 15 & 1));
  }
  return false;
}

// k blue i red j green
// ctrl x -> red facing me

GLuint chunk::RenderFace(std::vector<GLint> &&position) {
  // 1 -> back face: 2 -> front face: 3 -> left face: 4 -> right face: 5 -> top
  // face: 6 -> bottom face
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
  // Generate a random seed
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 1000000);
  int randomSeed = dist(gen);

  // 4 5 6 7
  // 0 1 2 3
  noise::module::RidgedMulti mountainTerrain;
  mountainTerrain.SetSeed(randomSeed);  // Set random seed for mountains
  noise::module::Billow baseFlatTerrain;
  baseFlatTerrain.SetFrequency(2.0);
  baseFlatTerrain.SetSeed(randomSeed);  // Set random seed for flat terrain

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
      // Extract the height value from the color's red channel (assuming height
      // is encoded in the red channel)
      int height = std::max(1, static_cast<int>((color.blue / 255.0f) * 32.0f));
      // Use the height map texture to get the height value of x, z
      for (int y = 0; y < CHUNK_BLOCK_COUNT; y++) {
        glm::ivec3 ofs = {z, y, x};
        blocks[z][y][x] = block(biomepos + ofs, y < height);  // mark them solid
      }
    }
  }
}

void chunk::Render(int setup) {
  // Rerendering
  rendervert.clear();
  cube_vertices.clear();
  cube_indices.clear();
  count = 0;
  if (!displaychunk) return;
  GLuint idx = 0;

  for (int i = 0; i < CHUNK_BLOCK_COUNT; i++) {
    for (int k = 0; k < CHUNK_BLOCK_COUNT; k++) {
      for (int j = 0; j < CHUNK_BLOCK_COUNT; j++) {
        // filled[0][0][0] = 1;
        if (!((blocks[i][j][k].blmask >> 15) & 1)) {
          // break;  // unsolid blocks
          continue;
        }
        GLuint mask = chunk::RenderFace({i, j, k});
        std::vector<GLuint> indices;
        std::vector<GLuint> blockrendervert;
        blocks[i][j][k].Render(mask, indices, blockrendervert);
        for (auto &ind : indices) ind += idx;
        rendervert.push_back({blockrendervert, indices});
        idx += 24, count++;
      }
    }
  }


  const GLuint cnt = count;
  const GLuint rsize = static_cast<GLuint>(rendervert.size());

  // std::vector<int> FrustumCull(rsize, 0);
  GLuint vcnt = 0, icnt = 0;

  for (GLuint i = 0; i < rsize; ++i) {
    auto &vert_ind = rendervert[i];
    const auto &verts = vert_ind.first;
    const auto &inds = vert_ind.second;

    vcnt += static_cast<GLuint>(verts.size());
    // Diabled for now //  TODO:
    // if (FrustumCulling(verts[0])) {
    //  FrustumCull[i] = 1;
    //  std::cout << "Frustum Culled!\n";
    //  continue;
    // }
    icnt += static_cast<GLuint>(inds.size());
  }

  cntblocks = icnt;
  cube_vertices.reserve(vcnt);
  cube_indices.reserve(icnt);

  for (GLuint i = 0; i < rsize; ++i) {
    // if (FrustumCull[i]) continue;
    auto &vert_ind = rendervert[i];
    const auto &verts = vert_ind.first;
    const auto &inds = vert_ind.second;

    cube_vertices.insert(cube_vertices.end(), verts.begin(), verts.end());
    cube_indices.insert(cube_indices.end(), inds.begin(), inds.end());
  }

  if (!setup) {
    chunkva->Bind();
    VertexBufferLayout layout;
    layout.Push(GL_UNSIGNED_INT, 1);
    VertexBuffer vb(cube_vertices.data(), cube_vertices.size() * sizeof(GLuint));
    chunkva->AddBuffer(vb, layout);
    IndexBuffer ib(cube_indices.data(), cube_indices.size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}

void chunk::Draw() {
  chunkva->Bind();
  glUniform3f(
      worldpos_uniform,
      CHUNK_BLOCK_COUNT * BLOCK_SIZE * (id / CHUNK_COUNTX),
      0.0,
      CHUNK_BLOCK_COUNT * BLOCK_SIZE * (id % CHUNK_COUNTZ));
  if (wireframemode) {
    // glUniform4f(vColor_uniform, 0.0, 0.0, 0.0, 1.0);
    glDrawElements(GL_LINES, cntblocks * 12 * 1, GL_UNSIGNED_INT, nullptr);
  } else {
    // glUniform4f(vColor_uniform, 0.5, 0.5, 0.5, 1.0);
    // 12 * Total Number of attributes
    glDrawElements(GL_TRIANGLES, cntblocks * 12 * 1, GL_UNSIGNED_INT, nullptr);
    // glUniform4f(vColor_uniform, 0.0, 0.0, 0.0, 1.0);
    glDrawElements(GL_LINES, cntblocks * 12 * 1, GL_UNSIGNED_INT, nullptr);
  }
}
