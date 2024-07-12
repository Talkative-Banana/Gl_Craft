#include "utils.h"

#define  GLM_FORCE_RADIANS
#define  GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <../stb/stb_image.h>
#include <../stb/stb_image_write.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Renderer.h"
#include "Texture.h"
#include "CameraController.h"
#include "TextureCubeMap.h"
#include "VertexArray.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "block.h"
#include "chunk.h"
#include "Biome.h"
#include "Input.h"
#include "main.h"

#define IMGUI_TEXT_CAPACITY 256

// Globals
float side = 2;
glm::vec3 worldpos;
int screen_width = 640, screen_height = 640;
GLint vModel_uniform, vView_uniform, vProjection_uniform, side_uniform, worldpos_uniform, vColor_uniform;
glm::mat4 modelT, viewT, projectionT; // The model, view and projection transformations
glm::vec3 camPosition;
char textKeyStatus[IMGUI_TEXT_CAPACITY];
char textKeyDescription[IMGUI_TEXT_CAPACITY];

void RenderBiome(unsigned int &, std::vector<VertexArray*> &, std::vector<unsigned int>&);
void createAxesLine(unsigned int &, unsigned int &);

void setupModelTransformationCube(unsigned int &);
void setupModelTransformationAxis(unsigned int &program, float rot_angle, glm::vec3 rot_axis);
void setupViewTransformation(unsigned int &, CameraController*);
void setupProjectionTransformation(unsigned int &, CameraController*);
void camTrans(glm::vec3 &);
Window* _window = nullptr;
CameraController *Camera;

int main(int, char**)
{
	// Setup window
	_window = new Window(screen_width, screen_height);
	GLFWwindow *window = _window->GetWindow();
	ImGuiIO& io = ImGui::GetIO(); // Create IO 
	ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

	unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");

	glUseProgram(shaderProgram);

	unsigned int axis_VAO;
	Camera = new CameraController(screen_height/screen_width);

	//Generate VAO object
	std::vector<VertexArray*> chunkva;
	std::vector<GLuint> cntblocks;

    GLint maxVAOs = 16;
    
    for (int i = 0; i < maxVAOs; ++i) {
		VertexArray *vao = new VertexArray(); vao->Bind();
        if (glGetError() == GL_OUT_OF_MEMORY) {
            std::cout << "Reached VAO limit at: " << i << std::endl;
            break;
        }
        chunkva.push_back(vao);
		glBindVertexArray(0);
    }

	GLuint Nokeypressed, wireframemode = 0;
	Material mat(0);
	TextureCubeMap tcm(mat.GetString());

	RenderBiome(shaderProgram, chunkva, cntblocks);
	createAxesLine(shaderProgram, axis_VAO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		Camera->CameraInputs();
		camPosition = Camera->GetCamera()->GetPosition();
		if (Input::IsKeyPressed(GLFW_KEY_TAB)) {
			strcpy(textKeyStatus, "TAB");
			strcpy(textKeyDescription, "Switching Mode");
			wireframemode ^= 1;
			Nokeypressed = 0;
		}

		if(Nokeypressed) { 
			strcpy(textKeyStatus, "Listening for key events...");
			strcpy(textKeyDescription, "Listening for key events...");
		}
			
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glUseProgram(shaderProgram);

		//ImGui UI menu
		ImGui::Begin("Main", NULL, ImGuiWindowFlags_AlwaysAutoResize);                          
		if(ImGui::CollapsingHeader("Information", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Key Status: %s", textKeyStatus);
			ImGui::Text("Key Description: %s", textKeyDescription);
			ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", camPosition.x, camPosition.y, camPosition.z);
		}
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		_window -> SetHeight(display_h); _window -> SetWidth(display_w);
		glViewport(0, 0, display_w, display_h);

		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE); // Enable OC
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);


		// Setup MVP matrix
		setupModelTransformationCube(shaderProgram);
		setupViewTransformation(shaderProgram, Camera);
		setupProjectionTransformation(shaderProgram, Camera);

		// glBindVertexArray(cube_VAO);
		tcm.Bind();
		for(int _chunkx = 0; _chunkx < 4; _chunkx++){
			for(int _chunky = 0; _chunky < 4; _chunky++){
				chunkva[4*_chunkx + _chunky]->Bind();

				worldpos_uniform = glGetUniformLocation(shaderProgram, "worldpos");
				if(worldpos_uniform == -1){
					fprintf(stderr, "Could not bind location: worldpos\n");
					exit(0);
				}
				glUniform3f(worldpos_uniform, 32 * side * _chunkx, 0.0, 32 * side * _chunky);
				if(wireframemode){
					// glUniform4f(vColor_uniform, 0.0, 0.0, 0.0, 1.0);
					glDrawElements(GL_LINES, cntblocks[4*_chunkx + _chunky] * 12 * 1, GL_UNSIGNED_INT, nullptr);
				} else{
					// glUniform4f(vColor_uniform, 0.5, 0.5, 0.5, 1.0);
					// 12 * Total Number of attributes
					glDrawElements(GL_TRIANGLES, cntblocks[4*_chunkx + _chunky] * 12 * 1, GL_UNSIGNED_INT, nullptr);
					// glUniform4f(vColor_uniform, 0.0, 0.0, 0.0, 1.0);
					glDrawElements(GL_LINES, cntblocks[4*_chunkx + _chunky] * 12 * 1, GL_UNSIGNED_INT, nullptr);
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
			}
		}

		glDisable(GL_DEPTH_TEST); // Disable depth test for drawing axes. We want axes to be drawn on top of all

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

		glEnable(GL_DEPTH_TEST); // Enable depth test again

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	for(auto &x : chunkva) delete x;
	delete Camera;
	cleanup(window);
	return 0;
}

int FrustumCulling(GLuint vertex){
	// return 0 if inside frustum

	auto Center = [&](glm::ivec3 pos, uint centeroff){
		glm::ivec3 center = pos;
	
		if((centeroff & 1u) == 1u){
			center.z += side / 2;
		} else{
			center.z -= side / 2;
		}
	
		if((centeroff & 2u) == 2u){
			center.y += side / 2;
		} else{
			center.y -= side / 2;
		}
	
		if((centeroff & 4u) == 4u){
			center.x += side / 2;
		} else{
			center.x -= side / 2;
		}
		return center;
	};

	uint positionX = (vertex)        & 63u;
	uint positionY = (vertex >> 6u)  & 63u;
	uint positionZ = (vertex >> 12u) & 63u;
	uint centeroff = (vertex >> 18u) & 7u;
	
	glm::ivec3 pos = glm::ivec3(side * positionX, side * positionY, side * positionZ);
	// Center of block
	glm::ivec3 centercord = Center(pos, centeroff);
	for(int i = 0; i < 6; i++){
		// for each of frusum face check if inside else return 1
		// TODO:
	}
	return 0;
}

void RenderBiome(unsigned int &program, std::vector<VertexArray*> &chunkva, std::vector<unsigned int>& cntblocks)
{
	glUseProgram(program);
	//Bind shader variables
	int vVertex_attrib = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}

	side_uniform = glGetUniformLocation(program, "side");
	if(side_uniform == -1){
		fprintf(stderr, "Could not bind location: side\n");
		exit(0);
	}
	glUniform1f(side_uniform, side);

	// Intialize Biome
	Biome b = Biome(1, glm::vec3(0, 0, 0), true);
	b.RenderBiome();

	// Create Layout for VAO
	// Position information (data + format)

	for(int _chunkx = 0; _chunkx < 4; _chunkx++){
		for(int _chunky = 0; _chunky < 4; _chunky++){
			// For Each Chunk Create and Bind Respective Vaos and Vbos and Ibos
			chunkva[4*_chunkx+_chunky]->Bind();
			//Chunk Data
			chunk* tmp = b.chunks[_chunkx][_chunky];
			GLuint vcnt = 0, icnt = 0, cnt = tmp-> count;
			GLuint rsize = tmp -> rendervert.size();

			std::vector<int> FrustumCull(rsize, 0);
			// Get Vertexcnt and indexcnt from the chunk per block
			for(int i = 0; i < rsize; i++){
				auto &[_vert, _ind] = tmp -> rendervert[i];
				vcnt += _vert.size(); 
				if(FrustumCulling(_vert[0])){
					FrustumCull[i] = 1; 
					continue;
				} 
				icnt += _ind.size();
			}
			// Allocate Heap memory for verticies and indices
			GLuint* cube_vertices = (GLuint*)malloc(vcnt * sizeof(GLuint));
			GLuint* cube_indices = (GLuint*)malloc(icnt * sizeof(GLuint));
			// Assign the memory
			int u = 0, v = 0;
			for(int i = 0; i < rsize; i++){
				if(FrustumCull[i]) continue; // Drop the block if outside frustum
				auto &[_vert, _ind] = tmp -> rendervert[i];
				for(int i = 0; i < _vert.size(); i++) cube_vertices[u++] = _vert[i];
				for(int i = 0; i < _ind.size(); i++) cube_indices[v++] = _ind[i];
			}

			std::cout << "[Memory Usage per chunk: " << (1.0 * (vcnt * sizeof(GLuint) + icnt * sizeof(GLuint))) / 1e6 << " mb]" << std::endl;

			//Create VBOs for the VAO
			int numofbytespervertex = 1;
			int numofvertexperblock = 24;
			int numofblocksperchunk = 32*32*32;
			cntblocks.push_back(icnt);

			VertexBufferLayout layout;
			layout.Push(GL_UNSIGNED_INT, 1);
			VertexBuffer vb(cube_vertices, cnt * numofbytespervertex * numofvertexperblock * sizeof(GLuint));
			// Add vb and layout to vao
			chunkva[4*_chunkx+_chunky]->AddBuffer(vb, layout);
			// Create IBOs for the VAO
			IndexBuffer ib(cube_indices, icnt);
			ib.Bind();
			delete []cube_vertices;
			delete []cube_indices;
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
		}
	}
}

void createAxesLine(unsigned int & program, unsigned int &axis_VAO) {
	glUseProgram(program);

	//Bind shader variables
	int vVertex_attrib_position = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib_position == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}

	//Axes data
	GLfloat axis_vertices[] = {	0, 0, 0, 20, 0, 0}; //X-axis
	glGenVertexArrays(1, &axis_VAO);
	glBindVertexArray(axis_VAO);

	//Create VBO for the VAO
	int nVertices = 2; // 2 vertices
	GLuint vertex_VBO;
	glGenBuffers(1, &vertex_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), axis_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib_position);
	glVertexAttribPointer(vVertex_attrib_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}

void setupModelTransformationCube(unsigned int &program) {
	//Modelling transformations (Model -> World coordinates)
	modelT = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 2.0, 1.0));
	modelT = glm::translate(modelT, glm::vec3(0.0f, 0.0f, 0.0f));

	//Pass on the modelling matrix to the vertex shader
	glUseProgram(program);
	vModel_uniform = glGetUniformLocation(program, "vModel");
	if(vModel_uniform == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupModelTransformationAxis(unsigned int &program, float rot_angle, glm::vec3 rot_axis) {
	//Modelling transformations (Model -> World coordinates)
	modelT = glm::rotate(glm::mat4(1.0f), rot_angle, rot_axis);

	//Pass on the modelling matrix to the vertex shader
	glUseProgram(program);
	vModel_uniform = glGetUniformLocation(program, "vModel");
	if(vModel_uniform == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}


void setupViewTransformation(unsigned int &program, CameraController* occ) {
	//Viewing transformations (World -> Camera coordinates
	// viewT = glm::lookAt(glm::vec3(camPosition), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	viewT = occ->GetCamera()->GetViewMatrix();

	//Pass-on the viewing matrix to the vertex shader
	glUseProgram(program);
	vView_uniform = glGetUniformLocation(program, "vView");
	if(vView_uniform == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}
	glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(unsigned int &program, CameraController* occ) {
	//Projection transformation
	projectionT = occ->GetCamera()->GetProjectionMatrix();

	//Pass on the projection matrix to the vertex shader
	glUseProgram(program);
	vProjection_uniform = glGetUniformLocation(program, "vProjection");
	if(vProjection_uniform == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}
