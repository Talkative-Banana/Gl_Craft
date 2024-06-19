#include "utils.h"

#define  GLM_FORCE_RADIANS
#define  GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>



#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "block.h"
#include "chunk.h"


//Globals
int screen_width = 640, screen_height = 640;
GLint vModel_uniform, vView_uniform, vProjection_uniform;
GLint vColor_uniform;
glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations
glm::vec4 camPosition;
char textKeyStatus[256];
float speed = 1.0f;
bool Perspective = false;

void RenderChunk(unsigned int &, unsigned int &, unsigned int&);
void createAxesLine(unsigned int &, unsigned int &);

void setupModelTransformationCube(unsigned int &);
void setupModelTransformationAxis(unsigned int &program, float rot_angle, glm::vec3 rot_axis);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &);
void camTrans(glm::vec3&);

int main(int, char**)
{
	// Setup window
	GLFWwindow *window = setupWindow(screen_width, screen_height);
	ImGuiIO& io = ImGui::GetIO(); // Create IO 
	ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	camPosition = glm::vec4(20.0, 40.0, 80.0, 1.0);

	unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
	//Get handle to color variable in shader
	vColor_uniform = glGetUniformLocation(shaderProgram, "vColor");
	if(vColor_uniform == -1){
		fprintf(stderr, "Could not bind location: vColor\n");
		exit(0);
	}

	glUseProgram(shaderProgram);

	// setupModelTransformation(shaderProgram);
	// Modelling transformation is setup in the display loop

	unsigned int cube_VAO, axis_VAO;
	GLuint cntblocks = 0;
	RenderChunk(shaderProgram, cube_VAO, cntblocks);
	createAxesLine(shaderProgram, axis_VAO);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

	glm::vec3 change(0.0f, 0.0f, 0.0f);
	// Get key presses
        if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
          strcpy(textKeyStatus, "Key status: Left");
			//TODO:
			change.x += speed;
        }
        else if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_RightArrow))) {
          strcpy(textKeyStatus, "Key status: Right");
			//TODO:
			change.x -= speed;
        }
        else if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
          if(io.KeyShift){
            strcpy(textKeyStatus, "Key status: Shift + Up");
			//TODO:
			change.z -= speed;
	  		}
			else{ 
				strcpy(textKeyStatus, "Key status: Up");
				//TODO:
				change.y += speed;
			}
        }
        else if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
			if(io.KeyShift){
				strcpy(textKeyStatus, "Key status: Shift + Down");
				//TODO:
				change.z += speed;
			} else{ 
				strcpy(textKeyStatus, "Key status: Down");
				//TODO:
				change.y -= speed;
			}
        }
        else if(ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Z))) {
			if(io.KeyCtrl){
				strcpy(textKeyStatus, "Key status: Ctrl + z");
				// Move camera to [0, 0, 100] i.e. => along z axis
				if(!Perspective || Perspective){
					camPosition = {0.0f, 0.0f, 100.0f, 1.0f};
				}
			} else{
					strcpy(textKeyStatus, "Key status: z");
					Perspective = false;
			}
        }
        else if(ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_X))) {
			if(io.KeyCtrl){
				strcpy(textKeyStatus, "Key status: Ctrl + x");
				// Move camera to [100, 0, 0] i.e. => along x axis
				if(!Perspective || Perspective){
					camPosition = {100.0f, 0.0f, 0.0f, 1.0f};
				}	
			} else{
					strcpy(textKeyStatus, "Key status: x");
					Perspective = true;
			}
		}
		else if(ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Y))){
			if(io.KeyCtrl){
				strcpy(textKeyStatus, "Key status: Ctrl + y");
				// Move camera to [0, 100, 0] i.e. => along y axis (due to camera rolling gaze direction shouldn't be parallel to y)
				// So Moved with some offset
				if(!Perspective || Perspective){
					camPosition = {0.0f, 100.0f, 0.20f, 1.0f};
				}
			}
		}
        else{ 
          strcpy(textKeyStatus, "Key status:");
		}	

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glUseProgram(shaderProgram);

		//ImGui UI menu
		ImGui::Begin("Main", NULL, ImGuiWindowFlags_AlwaysAutoResize);                          
		if(ImGui::CollapsingHeader("Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
		  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("%s", textKeyStatus);
			ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", camPosition.x, camPosition.y, camPosition.z);
		}
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Setup MVP matrix
		setupModelTransformationCube(shaderProgram);
		camTrans(change);
		setupViewTransformation(shaderProgram);
		setupProjectionTransformation(shaderProgram);

		glBindVertexArray(cube_VAO); 
		glUniform4f(vColor_uniform, 0.5, 0.5, 0.5, 1.0);
		glDrawElements(GL_TRIANGLES, cntblocks * 6 * 2 * 3, GL_UNSIGNED_INT, nullptr);
		glUniform4f(vColor_uniform, 0.0, 0.0, 0.0, 1.0);
		glDrawElements(GL_LINE_STRIP, cntblocks * 6 * 2 * 3, GL_UNSIGNED_INT, nullptr);

		glDisable(GL_DEPTH_TEST); // Disable depth test for drawing axes. We want axes to be drawn on top of all

		glBindVertexArray(axis_VAO); 
		setupModelTransformationAxis(shaderProgram, 0.0, glm::vec3(0, 0, 1));
		glUniform4f(vColor_uniform, 1.0, 0.0, 0.0, 1.0); //Red -> X 
		glDrawArrays(GL_LINES, 0, 2);

		setupModelTransformationAxis(shaderProgram, glm::radians(90.0), glm::vec3(0, 0, 1));
		glUniform4f(vColor_uniform, 0.0, 1.0, 0.0, 1.0); //Green -> Y
		glDrawArrays(GL_LINES, 0, 2);

		setupModelTransformationAxis(shaderProgram, -glm::radians(90.0), glm::vec3(0, 1, 0));
		glUniform4f(vColor_uniform, 0.0, 0.0, 1.0, 1.0); //Blue -> Z
		glDrawArrays(GL_LINES, 0, 2);

		glEnable(GL_DEPTH_TEST); // Enable depth test again

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	cleanup(window);

	return 0;
}

void camTrans(glm::vec3& Change)
{
// 1) Move from World Space to Camera Space
// 2) Perform Translation based on key values
// 3) Move back to World Space

	// uvw of camera space
	glm::vec3 t = {0.0f, 1.0f, 0.0f};
	glm::vec3 w = {camPosition.x, camPosition.y, camPosition.z};
	w = glm::normalize(w);
	glm::vec3 u = glm::normalize(glm::cross(t, w));
	glm::vec3 v = glm::normalize(glm::cross(w, u));

	//Traslation Matrix
	glm::mat4 matT = {1, 0, 0, 0, 
			  0, 1, 0, 0,
			  0, 0, 1, 0,
			  Change.x, Change.y, Change.z, 1};

	// XYZ wrt UVW
	glm::mat3 temp = {u.x, v.x, w.x, 
		          u.y, v.y, w.y,
		          u.z, v.z, w.z};

  // Solution of 3x3 XYZ matrix
	glm::mat3 rev = glm::inverse(temp);

	glm::vec3 X = glm::normalize(rev[0]);
  glm::vec3 Y = glm::normalize(rev[1]);
	glm::vec3 Z = glm::normalize(rev[2]);	


	// World to Camera Matrix Rotation
	glm::mat4 w2cr = {X.x, Y.x, Z.x, 0, 
			  X.y, Y.y, Z.y, 0,
			  X.z, Y.z, Z.z, 0,
			  0, 0, 0, 1};

	// World to Camera Matrix Transaltion
	glm::mat4 w2ct = {1, 0, 0, 0, 
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 -camPosition.x, -camPosition.y, -camPosition.z, 1};
	
	// World to Camera Matrix
	glm::mat4 w2c = w2cr * w2ct;
	
	// New Adjusted vector involving key inputs
	glm::vec4 adjustedvec = matT * w2c * camPosition;

	// Camera to World Matrix Rotation
	glm::mat4 c2wr = {u.x, u.y, u.z, 0, 
			  v.x, v.y, v.z, 0,
			  w.x, w.y, w.z, 0,
			  0, 0, 0, 1};

  // Camera to World Matrix Transalation
	glm::mat4 c2wt = {1, 0, 0, 0, 
			  0, 1, 0, 0,
			  0, 0, 1, 0,
			  adjustedvec.x, adjustedvec.y, adjustedvec.z, 1};

	// Camera to World Matrix
	glm::mat4 c2w = c2wr * c2wt;
	// Change vector in camPosition
	glm::vec4 newcampos = c2w * adjustedvec;

	camPosition.x += newcampos.x;
	camPosition.y += newcampos.y;
	camPosition.z += newcampos.z;
}

void RenderChunk(unsigned int &program, unsigned int &cube_VAO, unsigned int& cntblocks)
{
	glUseProgram(program);
	//Bind shader variables
	int vVertex_attrib = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}

	// Chunk
	glm::vec3 pos = {0.0, 0.0, 0.0};
	chunk c = chunk(20.0, pos, true);
	c.Render();
	//Cube data
	GLuint vcnt = c.rendervert.size(), icnt = c.indices.size(), cnt = c.count;

	float* cube_vertices = (float*)malloc(vcnt * sizeof(float));
	GLuint* cube_indices = (GLuint*)malloc(icnt * sizeof(GLuint));
	
	for(int i = 0; i < vcnt; i++) cube_vertices[i] = c.rendervert[i];
	for(int i = 0; i < icnt; i++) cube_indices[i] = c.indices[i];
	
	//Generate VAO object
	glGenVertexArrays(1, &cube_VAO);
	glBindVertexArray(cube_VAO);

	//Create VBOs for the VAO
	VertexBuffer vb(cube_vertices, cnt * 8 * 3 * sizeof(GLfloat));
	// Position information (data + format)
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);

	IndexBuffer ib(cube_indices, icnt);
	ib.Bind();
	cntblocks = cnt;
	// delete []expanded_vertices;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}

void createAxesLine(unsigned int & program, unsigned int &axis_VAO)
{
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

void setupModelTransformationCube(unsigned int &program)
{
	//Modelling transformations (Model -> World coordinates)
	modelT = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 2.0, 1.0));
	modelT = glm::translate(modelT, glm::vec3(0.0f, 10.0f, 0.0f));

	//Pass on the modelling matrix to the vertex shader
	glUseProgram(program);
	vModel_uniform = glGetUniformLocation(program, "vModel");
	if(vModel_uniform == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupModelTransformationAxis(unsigned int &program, float rot_angle, glm::vec3 rot_axis)
{
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


void setupViewTransformation(unsigned int &program)
{
	//Viewing transformations (World -> Camera coordinates
	viewT = glm::lookAt(glm::vec3(camPosition), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	//Pass-on the viewing matrix to the vertex shader
	glUseProgram(program);
	vView_uniform = glGetUniformLocation(program, "vView");
	if(vView_uniform == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}
	glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(unsigned int &program)
{
	//Projection transformation
	if(Perspective){
		projectionT = glm::perspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 0.1f, 1000.0f);
	} else{
		//Window Size => l, r, b, u, n, f
			projectionT = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -5000.0f, 5000.0f);
	}

	//Pass on the projection matrix to the vertex shader
	glUseProgram(program);
	vProjection_uniform = glGetUniformLocation(program, "vProjection");
	if(vProjection_uniform == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}
