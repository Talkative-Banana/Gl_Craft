#include "Renderer.h"
#include "chunk.h"
#include <cstdlib>
#include <string>

chunk::chunk(uint _id, glm::ivec3 position, GLboolean display){
    id = _id;
    count = 0;
    chunkpos = position;
    displaychunk = display;
    memset(filled, 0, sizeof filled);
}

 GLboolean chunk::isSolid(std::vector<GLint> position){
    if((position[0] >= 0) && (position[0] < 32) && (position[1] >= 0) && (position[1] < 32) && (position[2] >= 0) && (position[2] < 32)){
        return filled[position[0]][position[1]][position[2]];
    }
    return false;
}
// k blue i red j green
// ctrl x -> red facing me

GLuint chunk::RenderFace(std::vector<GLint> position){
    // 1 -> back face: 2 -> front face: 3 -> left face: 4 -> right face: 5 -> top face: 6 -> bottom face
    GLuint mask = 0;
    std::vector<GLint> tmp = position;
    for(GLuint face = 1; face <= 6; face++){
        if(face == 1){
            // No Need to draw back face if block behind is solid
            tmp[2] -= 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[2] += 1;
        } else if(face == 2){
            // No Need to draw front face if block in front is solid
            tmp[2] += 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[2] -= 1;
        } else if(face == 3){
            // No Need to draw left face if block in left is solid
            tmp[0] -= 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[0] += 1;
        } else if(face == 4){
            // No Need to draw right face if block in right is solid
            tmp[0] += 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[0] -= 1;
        } else if(face == 5){
            // No Need to draw top face if block on top is solid
            tmp[1] += 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[1] -= 1;
        } else if(face == 6){
            // No Need to draw bottom face if block on bottom is solid
            tmp[1] -= 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[1] += 1;
        }
    }
    return mask;
}

void chunk::Setup_Landscape(GLint X, GLint Z) {

    // 4 5 6 7
    // 0 1 2 3
    noise::module::Perlin myModule;
    noise::utils::NoiseMap heightMap;
    noise::utils::NoiseMapBuilderPlane heightMapBuilder;
    heightMapBuilder.SetSourceModule (myModule);
    heightMapBuilder.SetDestNoiseMap (heightMap);
    heightMapBuilder.SetDestSize (256, 256);
    heightMapBuilder.SetBounds (4.0 * Z, 4.0 * (Z + 1), 4.0 * X, 4.0 * (X + 1));
    heightMapBuilder.Build();

    noise::utils::RendererImage renderer;
    noise::utils::Image image;
    renderer.SetSourceNoiseMap (heightMap);
    renderer.SetDestImage (image);
    renderer.Render();

    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) { // Use the noise library to get the height value of x, z             
            noise::utils::Color color = image.GetValue(x, z);
            // Extract the height value from the color's red channel (assuming height is encoded in the red channel)
            int height = std::max(1, static_cast<int>((color.blue / 255.0f) * 32.0f));
            // Use the height map texture to get the height value of x, z  
            for (int y = 0; y < height; y++) {
                filled[z][y][x] = 1;
            }
        }
    }
}

void chunk::Render(){
    if(!displaychunk) return;
    GLuint idx = 0;
    
    Setup_Landscape(chunkpos.x, chunkpos.z);

    for(int i = 0; i < 32; i++){
        for(int k = 0; k < 32; k++){
            for(int j = 0; j < 32; j++){
                // filled[0][0][0] = 1;
                if(!filled[i][j][k]) break;
                glm::ivec3 ofs = {i, j, k};
                std::vector<GLint> Idx = {i, j, k};
                GLuint mask = chunk::RenderFace(Idx);
                blocks[i][j][k] = new block(ofs, true); blocks[i][j][k]->Render(mask);
                std::vector<GLuint> blockrendervert = blocks[i][j][k]->rendervert;
                std::vector<GLuint> blockindices = blocks[i][j][k]->indices;
                for(auto &ind : blockindices) ind += idx;
                rendervert.push_back({blockrendervert, blockindices});
                idx += 24, count++;
            }
        }
    }
}

chunk::~chunk() {}
