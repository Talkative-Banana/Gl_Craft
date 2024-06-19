#include "Renderer.h"
#include "chunk.h"
#include "block.h"

chunk::chunk(bool display, glm::vec3 position){
    displaychunk = display;
    chunkpos = position;
    memset(filled, 0, sizeof filled);
}

chunk::~chunk(){}