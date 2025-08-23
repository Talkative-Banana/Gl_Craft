#pragma once
constexpr static float BLOCK_SIZE = 2.0;  // HALF BLOCK LENGTH
constexpr static float NEAR_PLANE = 1.0f;
constexpr static float FAR_PLANE = 1000.0f;
constexpr static int TOTAL_STEPS = 32;
constexpr static float STEP_SIZE = 1.0;
constexpr static int CHUNK_COUNTX = 4;
constexpr static int BIOME_COUNTX = 1;
constexpr static int CHUNK_COUNTY = 4;
constexpr static int BIOME_COUNTY = 1;
constexpr static int CHUNK_BLOCK_COUNT = 32;
constexpr static int GRASS_BLOCK = 0;
constexpr static int DIRT_BLOCK = 1;
constexpr static int SAND_BLOCK = 2;
constexpr static int COBBOLESTONE_BLOCK = 3;
constexpr static float OFFSET = 0.01f;

// Non const
static int SCREEN_HEIGHT = 640;
static int SCREEN_WIDTH = 640;
