#include "Utils.h"

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <../stb/stb_image.h>
#include <../stb/stb_image_write.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

#include "AssetManager.h"
#include "CameraController.h"
#include "Constants.hpp"
#include "IndexBuffer.h"
#include "Input.h"
#include "Main.h"
#include "Material.h"
#include "Player.h"
#include "Ray.h"
#include "Renderer.h"
#include "Texture.h"
#include "TextureCubeMap.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "World.h"
#define IMGUI_TEXT_CAPACITY 256

// Globals
glm::ivec3 _wps = {0, 0, 0};
std::unique_ptr<World> world = std::make_unique<World>(42, _wps);
glm::vec3 chunkpos;
GLint vModel_uniform, vView_uniform, vProjection_uniform, side_uniform, chunkpos_uniform,
    vColor_uniform, atlas_uniform;
glm::mat4 modelT, viewT,
    projectionT;  // The model, view and projection transformations
char textKeyStatus[IMGUI_TEXT_CAPACITY];
char textKeyDescription[IMGUI_TEXT_CAPACITY];
GLuint Nokeypressed, wireframemode = 0, activePlayer = 0, players_cnt = 2;
std::array<std::unique_ptr<Player>, PLAYER_COUNT> players;

void createAxesLine(unsigned int &, unsigned int &);

void setupModelTransformationCube(unsigned int &);
void setupModelTransformationAxis(unsigned int &program, float rot_angle, glm::vec3 rot_axis);
void setupViewTransformation(unsigned int &, std::unique_ptr<CameraController> &);
void setupProjectionTransformation(unsigned int &, std::unique_ptr<CameraController> &);
void camTrans(glm::vec3 &);
Window *_window = nullptr;
bool enable_gravity = 1;

void draw_axis(unsigned int axis_VAO, unsigned int shaderProgram) {
  glBindVertexArray(axis_VAO);
  setupModelTransformationAxis(shaderProgram, 0.0, glm::vec3(0, 0, 1));
  // glUniform4f(vColor_uniform, 1.0, 0.0, 0.0, 1.0); //Red -> X
  glDrawArrays(GL_LINES, 0, 2);

  setupModelTransformationAxis(shaderProgram, glm::radians(90.0), glm::vec3(0, 0, 1));
  // glUniform4f(vColor_uniform, 0.0, 1.0, 0.0, 1.0); //Green -> Y
  glDrawArrays(GL_LINES, 0, 2);

  setupModelTransformationAxis(shaderProgram, -glm::radians(90.0), glm::vec3(0, 1, 0));
  // glUniform4f(vColor_uniform, 0.0, 0.0, 1.0, 1.0); //Blue -> Z
  glDrawArrays(GL_LINES, 0, 2);

  glEnable(GL_DEPTH_TEST);  // Enable depth test again
}

int main(int, char **) {
  // Size of Structs
  std::cout << "Size of block is: " << sizeof(Block) << std::endl;
  std::cout << "Size of chunk is: " << sizeof(Chunk) << std::endl;
  std::cout << "Size of biome is: " << sizeof(Biome) << std::endl;
  // Setup window
  _window = new Window(SCREEN_WIDTH, SCREEN_HEIGHT);
  GLFWwindow *window = _window->GetWindow();
  ImGuiIO &io = ImGui::GetIO();  // Create IO
  ImVec4 clearColor = ImVec4(0.471f, 0.786f, .784f, 1.00f);

  unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
  unsigned int shaderProgram2 = createProgram("./shaders/vshader2.vs", "./shaders/fshader2.fs");

  glUseProgram(shaderProgram);

  unsigned int axis_VAO;

  Texture atlas("textures/default_texture.png");

  static int vVertex_attrib = -1;
  static int side_uniform = -1;
  static int vNormal_attrib = -1;
  static int cameraPos_uniform = -1;
  static int lightpos_uniform = -1;
  static int atlast_uniform = -1;
  if (vVertex_attrib == -1) {
    vVertex_attrib = glGetAttribLocation(shaderProgram, "vVertex");
    if (vVertex_attrib == -1) {
      fprintf(stderr, "Could not bind location: vVertex\n");
      exit(0);
    }
  }

  if (vNormal_attrib == -1) {
    vNormal_attrib = glGetAttribLocation(shaderProgram2, "vNormal");
    if (vNormal_attrib == -1) {
      std::cout << "Could not bind location: vNormal\n";
      exit(0);
    }
  }

  // Get handle to eye normal variable in shader
  cameraPos_uniform = glGetUniformLocation(shaderProgram2, "cameraPos");
  if (cameraPos_uniform == -1) {
    fprintf(stderr, "Could not bind location: cameraPos. Specular Lighting Switched Off.\n");
  }

  // Moved outside of loop
  lightpos_uniform = glGetUniformLocation(shaderProgram2, "lightpos");
  if (lightpos_uniform == -1) {
    fprintf(stderr, "Could not bind location: lightpos\n");
    exit(0);
  }

  if (side_uniform == -1) {
    side_uniform = glGetUniformLocation(shaderProgram, "side");
    if (side_uniform == -1) {
      fprintf(stderr, "Could not bind location: side\n");
      exit(0);
    }
  }

  atlas_uniform = glGetUniformLocation(shaderProgram, "atlas");
  if (atlas_uniform == -1) {
    std::cerr << "Could not bind: atlas\n";
    exit(0);
  }
  glUniform1f(side_uniform, BLOCK_SIZE);


  createAxesLine(shaderProgram, axis_VAO);

  // Moved outside of loop
  chunkpos_uniform = glGetUniformLocation(shaderProgram, "chunkpos");
  if (chunkpos_uniform == -1) {
    fprintf(stderr, "Could not bind location: chunkpos\n");
    exit(0);
  }

  // Initalize all the players
  for (int i = 0; i < players_cnt; i++) {
    players[i] = std::make_unique<Player>();
  }

  std::unique_ptr<AssetManager> asset_manager = std::make_unique<AssetManager>();

  float asset_height = 70.0;

  uint64_t handle1 = asset_manager->loadMeshObject(
      "assets/bunny.obj",
      shaderProgram2,
      0.025,
      0.0,
      glm::vec3(70.0, asset_height, 70.0),
      glm::vec3(1.0, 0.0, 0.0));
  uint64_t handle2 = asset_manager->loadMeshObject(
      "assets/buddha.obj",
      shaderProgram2,
      0.025,
      180.0,
      glm::vec3(70, asset_height, 100.0),
      glm::normalize(glm::vec3(0.0, 1.0, 1.0)));

  auto mesh1 = asset_manager->get_mesh(handle1);
  auto mesh2 = asset_manager->get_mesh(handle2);
  mesh1->setup();
  mesh2->setup();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // handle player input
    players[activePlayer]->handle_input();
    // World Calculations
    world->SetupWorld(players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition());
    world->Update_queue(players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition());

    if (Input::WasKeyPressed(GLFW_KEY_TAB)) {
      strcpy(textKeyStatus, "TAB");
      strcpy(textKeyDescription, "Switching Mode");
      wireframemode ^= 1;
      Nokeypressed = 0;
    }

    auto get_neighbors = [](glm::ivec3 vec) -> std::vector<std::shared_ptr<Chunk>> {
      std::shared_ptr<Chunk> left, front, right, back;
      left = world->get_chunk_by_center(
          vec + glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
      front = world->get_chunk_by_center(
          vec + glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
      right = world->get_chunk_by_center(
          vec - glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
      back = world->get_chunk_by_center(
          vec - glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
      return {left, front, right, back};
    };

    if (Input::WasMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      strcpy(textKeyStatus, "Right click");
      strcpy(textKeyDescription, "Casting ray");

      double mouseX, mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);

      Ray ray = screenPosToWorldRay(window, mouseX, mouseY, viewT, projectionT);

      if (ray.did_hit(world)) {  // Remove a block
        std::cout << "Ray hit a block with center: " << ray.m_hitcords.x << " " << ray.m_hitcords.y
                  << " " << ray.m_hitcords.z << std::endl;
        auto block = world->get_block_by_center(ray.m_hitcords);
        if (block) block->remove();
        auto _chunk = world->get_chunk_by_center(ray.m_hitcords);
        auto _biome = world->get_biome_by_center(ray.m_hitcords);
        // Set the dirty bit
        _chunk->dirtybit = true;
        _biome->dirtybit = true;
        auto vec = ray.m_hitcords;

        int cordz = (ray.m_hitcords.z % static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE));
        int cordx = (ray.m_hitcords.x % static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE));
        // If last block update adjacent chunk
        auto chunks1 = get_neighbors(vec);
        _chunk->Render(0, true, nullptr, nullptr, nullptr, nullptr);

        bool update_boundary = false;
        if (cordz == static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE) - 1) {
          auto chunks2 = get_neighbors(
              vec + glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
          if (chunks1[1]) {
            std::cout << "[FRONT] Updating neighbouring chunk\n";
            chunks1[1]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[1]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }

        if (cordx == static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE) - 1) {
          auto chunks2 = get_neighbors(
              vec + glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
          if (chunks1[0]) {
            std::cout << "[LEFT] Updating neighbouring chunk\n";
            chunks1[0]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[0]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }

        if (cordz == 1) {
          auto chunks2 = get_neighbors(
              vec - glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
          if (chunks1[3]) {
            std::cout << "[BACK] Updating neighbouring chunk\n";
            chunks1[3]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[3]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }

        if (cordx == 1) {
          auto chunks2 = get_neighbors(
              vec - glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
          if (chunks1[2]) {
            std::cout << "[RIGHT] Updating neighbouring chunk\n";
            chunks1[2]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[2]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }
        _chunk->Render(0, false, chunks1[0], chunks1[1], chunks1[2], chunks1[3]);

        // world->RenderWorld();
      } else {
        std::cout << "Ray didn't hit any block\n";
      }
    } else if (Input::WasMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {  // Add a block
      strcpy(textKeyStatus, "Middle click");
      strcpy(textKeyDescription, "Casting ray");

      double mouseX, mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);

      Ray ray = screenPosToWorldRay(window, mouseX, mouseY, viewT, projectionT);

      if (ray.did_hit(world)) {
        std::cout << "Ray hit a block with center: " << ray.m_hitcords.x << " " << ray.m_hitcords.y
                  << " " << ray.m_hitcords.z << std::endl;
        glm::ivec3 prev_blk = ray.m_hitcords + ray.m_hitnormal * static_cast<int>(BLOCK_SIZE);
        auto block = world->get_block_by_center(prev_blk);
        if (block) block->add();
        auto _chunk = world->get_chunk_by_center(ray.m_hitcords);
        auto _biome = world->get_biome_by_center(ray.m_hitcords);
        // Update dirty bit
        _chunk->dirtybit = true;
        _biome->dirtybit = true;

        auto vec = ray.m_hitcords;

        int cordz = (ray.m_hitcords.z % static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE));
        int cordx = (ray.m_hitcords.x % static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE));
        // If last block update adjacent chunk
        auto chunks1 = get_neighbors(vec);
        _chunk->Render(0, true, nullptr, nullptr, nullptr, nullptr);

        if (cordz == static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE) - 1) {
          auto chunks2 = get_neighbors(
              vec + glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
          if (chunks1[1]) {
            std::cout << "[FRONT] Updating neighbouring chunk\n";
            chunks1[1]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[1]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }

        if (cordx == static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE) - 1) {
          auto chunks2 = get_neighbors(
              vec + glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
          if (chunks1[0]) {
            std::cout << "[LEFT] Updating neighbouring chunk\n";
            chunks1[0]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[0]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }

        if (cordz == 1) {
          auto chunks2 = get_neighbors(
              vec - glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
          if (chunks1[3]) {
            std::cout << "[BACK] Updating neighbouring chunk\n";
            chunks1[3]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[3]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }

        if (cordx == 1) {
          auto chunks2 = get_neighbors(
              vec - glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
          if (chunks1[2]) {
            std::cout << "[RIGHT] Updating neighbouring chunk\n";
            chunks1[2]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
            chunks1[2]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
          }
        }
        _chunk->Render(0, false, chunks1[0], chunks1[1], chunks1[2], chunks1[3]);
      } else {
        std::cout << "Ray didn't hit any block\n";
      }
    }

    if (Input::WasKeyPressed(GLFW_KEY_P)) {
      // Switch Player
      activePlayer += 1;
      activePlayer %= players_cnt;
    }

    if (Input::WasKeyPressed(GLFW_KEY_K)) {
      asset_height -= BLOCK_SIZE;
      mesh1->pos.y = asset_height;
      mesh2->pos.y = asset_height;
    } else if (Input::WasKeyPressed(GLFW_KEY_L)) {
      asset_height += BLOCK_SIZE;
      mesh1->pos.y = asset_height;
      mesh2->pos.y = asset_height;
    }

    if (Nokeypressed) {
      strcpy(textKeyStatus, "Listening for key events...");
      strcpy(textKeyDescription, "Listening for key events...");
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui UI menu
    ImGui::Begin("Main", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Information", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text(
          "%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::Text("Key Status: %s", textKeyStatus);
      ImGui::Text("Key Description: %s", textKeyDescription);
      ImGui::Text("Active Player: %d", activePlayer);
      ImGui::Text(
          "Player %d position: (%.2f, %.2f, %.2f)",
          activePlayer,
          players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().x,
          players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().y,
          players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().z);
    }
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    _window->SetHeight(display_h);
    _window->SetWidth(display_w);
    players[activePlayer]->m_cameracontroller->SetAspectRatio((float)display_w / (float)display_h);
    glViewport(0, 0, display_w, display_h);

    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);  // Enable OC
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    mesh1->render(players[activePlayer]->m_cameracontroller);
    mesh2->render(players[activePlayer]->m_cameracontroller);
    glUniform3f(
        lightpos_uniform,
        players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().x,
        players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().y,
        players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().z);

    glUniform3f(
        cameraPos_uniform,
        players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().x,
        players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().y,
        players[activePlayer]->m_cameracontroller->GetCamera()->GetPosition().z);

    glUseProgram(shaderProgram);
    // Setup MVP matrix
    setupModelTransformationCube(shaderProgram);
    setupViewTransformation(shaderProgram, players[activePlayer]->m_cameracontroller);
    setupProjectionTransformation(shaderProgram, players[activePlayer]->m_cameracontroller);

    // glBindVertexArray(cube_VAO);
    atlas.Bind();
    glUniform1i(atlas_uniform, 0);  // bind sampler to texture unit 0
    world->Draw();
    glDisable(GL_DEPTH_TEST);  // Disable depth test for drawing axes. We want
                               // axes to be drawn on top of all

    draw_axis(axis_VAO, shaderProgram);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  world->save();
  // Cleanup
  cleanup(window);
  return 0;
}

int FrustumCulling(GLuint vertex) {
  // return 0 if inside frustum

  auto Center = [&](glm::ivec3 pos, uint centeroff) {
    glm::ivec3 center = pos;

    if ((centeroff & 1u) == 1u) {
      center.z += HALF_BLOCK_SIZE;
    } else {
      center.z -= HALF_BLOCK_SIZE;
    }

    if ((centeroff & 2u) == 2u) {
      center.y += HALF_BLOCK_SIZE;
    } else {
      center.y -= HALF_BLOCK_SIZE;
    }

    if ((centeroff & 4u) == 4u) {
      center.x += HALF_BLOCK_SIZE;
    } else {
      center.x -= HALF_BLOCK_SIZE;
    }
    return center;
  };

  uint positionX = (vertex) & 63u;
  uint positionY = (vertex >> 6u) & 63u;
  uint positionZ = (vertex >> 12u) & 63u;
  uint centeroff = (vertex >> 18u) & 7u;

  glm::ivec3 pos =
      glm::ivec3(BLOCK_SIZE * positionX, BLOCK_SIZE * positionY, BLOCK_SIZE * positionZ);
  // Center of block
  glm::ivec3 centercord = Center(pos, centeroff);
  for (int i = 0; i < 6; i++) {
    // for each of frusum face check if inside else return 1
    // TODO:
  }
  return 0;
}

void createAxesLine(unsigned int &program, unsigned int &axis_VAO) {
  glUseProgram(program);

  // Bind shader variables
  int vVertex_attrib_position = glGetAttribLocation(program, "vVertex");
  if (vVertex_attrib_position == -1) {
    fprintf(stderr, "Could not bind location: vVertex\n");
    exit(0);
  }

  // Axes data
  GLfloat axis_vertices[] = {0, 0, 0, 20, 0, 0};  // X-axis
  glGenVertexArrays(1, &axis_VAO);
  glBindVertexArray(axis_VAO);

  // Create VBO for the VAO
  int nVertices = 2;  // 2 vertices
  GLuint vertex_VBO;
  glGenBuffers(1, &vertex_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
  glBufferData(GL_ARRAY_BUFFER, nVertices * 3 * sizeof(GLfloat), axis_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vVertex_attrib_position);
  glVertexAttribPointer(vVertex_attrib_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);  // Unbind the VAO to disable changes outside this function.
}

void setupModelTransformationCube(unsigned int &program) {
  // Modelling transformations (Model -> World coordinates)
  modelT = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0));
  modelT = glm::translate(modelT, glm::vec3(0.0f, 0.0f, 0.0f));

  // Pass on the modelling matrix to the vertex shader
  glUseProgram(program);
  vModel_uniform = glGetUniformLocation(program, "vModel");
  if (vModel_uniform == -1) {
    fprintf(stderr, "Could not bind location: vModel\n");
    exit(0);
  }
  glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupModelTransformationAxis(unsigned int &program, float rot_angle, glm::vec3 rot_axis) {
  // Modelling transformations (Model -> World coordinates)
  modelT = glm::rotate(glm::mat4(1.0f), rot_angle, rot_axis);

  // Pass on the modelling matrix to the vertex shader
  glUseProgram(program);
  vModel_uniform = glGetUniformLocation(program, "vModel");
  if (vModel_uniform == -1) {
    fprintf(stderr, "Could not bind location: vModel\n");
    exit(0);
  }
  glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupViewTransformation(unsigned int &program, std::unique_ptr<CameraController> &occ) {
  // Viewing transformations (World -> Camera coordinates
  //  viewT = glm::lookAt(glm::vec3(camPosition), glm::vec3(0.0, 0.0, 0.0),
  //  glm::vec3(0.0, 1.0, 0.0));
  viewT = occ->GetCamera()->GetViewMatrix();

  // Pass-on the viewing matrix to the vertex shader
  glUseProgram(program);
  vView_uniform = glGetUniformLocation(program, "vView");
  if (vView_uniform == -1) {
    fprintf(stderr, "Could not bind location: vView\n");
    exit(0);
  }
  glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(unsigned int &program, std::unique_ptr<CameraController> &occ) {
  // Projection transformation
  projectionT = occ->GetCamera()->GetProjectionMatrix();

  // Pass on the projection matrix to the vertex shader
  glUseProgram(program);
  vProjection_uniform = glGetUniformLocation(program, "vProjection");
  if (vProjection_uniform == -1) {
    fprintf(stderr, "Could not bind location: vProjection\n");
    exit(0);
  }
  glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}
