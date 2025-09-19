#pragma once
constexpr static float BLOCK_SIZE = 2.0;       // BLOCK LENGTH
constexpr static float HALF_BLOCK_SIZE = 1.0;  // HALF_BLOCK LENGTH
constexpr static float NEAR_PLANE = 0.01f;
constexpr static float FAR_PLANE = 1000.0f;
constexpr static int TOTAL_STEPS = 32;
constexpr static int RENDER_DISTANCE = 128;
constexpr static float STEP_SIZE = 1.0;
constexpr static int CHUNK_COUNTX = 4;
constexpr static int BIOME_COUNTX = 1024;
constexpr static int CHUNK_COUNTZ = 4;
constexpr static int BIOME_COUNTZ = 1024;
constexpr static int CHUNK_BLOCK_COUNT = 32;
constexpr static int DEFAULT_TEXTURE = 0;
constexpr static int GRASS_BLOCK = 1;
constexpr static int DIRT_BLOCK = 2;
constexpr static int SAND_BLOCK = 3;
constexpr static int COBBOLESTONE_BLOCK = 4;
constexpr static int SNOW_BLOCK = 5;
constexpr static float OFFSET = 0.01f;
constexpr static float PLAYER_HEIGHT = 2 * BLOCK_SIZE;
constexpr static float GRAVITY = 0.98f;
constexpr static int PLAYER_COUNT = 128;

constexpr static int BACK_FACE = 1 << 17;
constexpr static int FRONT_FACE = 1 << 18;
constexpr static int LEFT_FACE = 1 << 19;
constexpr static int RIGHT_FACE = 1 << 20;
constexpr static int FACE_MASK_BITS = 6;
constexpr static int FACE_MASK = ((1 << FACE_MASK_BITS) - 1) << 17;

// Non const
static int SCREEN_HEIGHT = 640;
static int SCREEN_WIDTH = 640;
