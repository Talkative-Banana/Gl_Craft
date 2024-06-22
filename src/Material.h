#pragma once

#include <array>
#include <string>

// 0 GrassBlock
// 1 DirtBlock 
// 2 SandBlock
 
class Material{
public:
    Material(unsigned int id);
    const std::string* GetString() const;
private:
    const std::string* data;
    std::string texturedata[3][6] = {
        // Grass Block
        {
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/grass_side_carried.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/grass_side_carried.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/grass_carried.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/grass_side_carried.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/grass_side_carried.png"
        },
        // Dirt Block
        {
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/dirt.png"
        },
        // Sand Block
        {
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/sand.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/sand.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/sand.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/sand.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/sand.png",
            "./textures/bedrock-samples-1.21.0.3/resource_pack/textures/blocks/sand.png"
        }
    };
};