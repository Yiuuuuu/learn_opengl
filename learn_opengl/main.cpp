#include <glad\glad.h>

#include <GLFW\glfw3.h>
#include <stb\stb_image.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(char const* path);

//Setting
unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

//Control
bool firstMouse = true;
int cursorLastX = SCREEN_WIDTH / 2;
int cursorLastY = SCREEN_HEIGHT / 2;

//Camera
camera camera_one(glm::vec3(0.0f, 0.0f, 3.0f));
camera& currentCamera = camera_one;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float pitch = 0.0f;
float yaw = -90.0f;		//Yaw = -90.0f means facing front
float fov = 45.0f;

//Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main() {
	#include "verticeData.h"
	//---------------------GLFW & GLAD initialize---------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//Set the version to 3 to both major & minor
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Set the profile as Core model instead of intermediate mode
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	//For Mac OS 
	#endif

	//---------------------Window Create---------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeLimits(window, SCREEN_WIDTH, SCREEN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetWindowOpacity(window, 0.5f);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

	//Function Setup
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);	//Compare the "W", identify which object is in the front, then draw properly
	//glDepthFunc(GL_ALWAYS);
	//Stencil:   Stencil buffer does not work while framebuffer is working, dont know why
	/*glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);*/
	//Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);  //This can replace the previous statement
	//Culling Face
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	//Default: GL_CCW
	glCullFace(GL_BACK);	//Default: GL_BACK

	//---------------------Class Shader---------------------
	shader modelShader("shaderSource/lightingVertexShader_0.vert", "shaderSource/lightingFragmentShader_0.frag");
	shader lampShader("shaderSource/lampVertexShader_0.vert", "shaderSource/lampFragmentShader_0.frag");
	shader bounderShader("shaderSource/stencilVertexShader.vert", "shaderSource/stencilFragmentShader.frag");
	shader boxShader("shaderSource/boxVertexShader.vert", "shaderSource/boxFragmentShader.frag");
	shader discardShader("shaderSource/boxVertexShader.vert", "shaderSource/discardFragmentShader.frag");
	shader imageShader("shaderSource/boxVertexShader.vert", "shaderSource/imageFragmentShader.frag");
	shader screenShader("shaderSource/framebufferVertexShader.vert", "shaderSource/framebufferFragmentShader.frag");
	//---------------------Image loading---------------------
	unsigned int boxSurface = loadTexture("image/ground.png");
	unsigned int grassSurface = loadTexture("image/grass.png");
	unsigned int windowSurface = loadTexture("image/window.png");
	//---------------------Model loading---------------------
	Model ourModel("model/nanosuit/nanosuit.obj");
	//---------------------Lamp---------------------
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCoord), &cubeCoord, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//---------------------Box---------------------
	unsigned int boxVAO;
	glGenVertexArrays(1, &boxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBindVertexArray(boxVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//---------------------Plane---------------------
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeCoord), &planeCoord, GL_STATIC_DRAW);

	glBindVertexArray(planeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//---------------------Framebuffer---------------------
	//Create framebuffer VAO for screen printing
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadCoord), &quadCoord, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	//Create fbo
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Create a color attachment texture
	unsigned int textureColorBuffer;
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//GL_REPEAT
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);//Attach texture buffer

	//Create rbo
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);//Attach rbo

	//Check complete the conditions or not? 1. At least one buffer (color/stencil/depth) 2. At least one attachment 3. All attachments keep their memory
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::printf("ERROR::FRAMEBUFFER: Framebuffer is NOT complete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Setup framebuffer shader
	screenShader.useProgram();
	screenShader.setInt("screenTexture", 0);

	//---------------------Rendering---------------------
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			//Change drawing mode, make it as a line

	modelShader.useProgram();
	modelShader.setInt("material.diffuse", 0);
	modelShader.setInt("material.specular", 1);
	modelShader.setFloat("material.shininess", 32.0f);
	glm::vec3 lightPos(3.0f, 1.0f, 0.0f);

	unsigned int fps = 0;
	float secondAccumulator = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		//Frame time & fps
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		secondAccumulator += deltaTime;
		fps++;
		if (secondAccumulator >= 1.0f) {
			secondAccumulator = 0.0f;
			std::printf("FPS: %d\n", fps);
			fps = 0;
		}
		//Input
		processInput(window);

		//Framebuffer Stage 1
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Clear
		/*glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);*/


		//Model & View & Projection
		glm::mat4 model			= glm::mat4(1.0f);
		glm::mat4 view			= glm::mat4(1.0f);
		glm::mat4 projection	= glm::mat4(1.0f);

		view = currentCamera.viewMatrix();
		projection = glm::perspective(glm::radians(currentCamera.zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

		//Lamp
		lampShader.useProgram();
		glStencilMask(0x00);

		lightPos.x = glm::cos(currentFrame) * 3.0f;
		lightPos.z = glm::sin(currentFrame) * 3.0f;

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.3f));
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("model", model);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		//boxes
		std::vector<glm::vec3> boxLocations;
		boxLocations.push_back(glm::vec3(3.0f, -3.0f, -5.0f));
		boxLocations.push_back(glm::vec3(-3.5f, -1.0f, -7.0f));
		boxLocations.push_back(glm::vec3(0.5f, -2.0f, -4.0f));

		boxShader.useProgram();
		boxShader.setMat4("view", view);
		boxShader.setMat4("projection", projection);
		glBindVertexArray(boxVAO);
		for (int i = 0; i < boxLocations.size(); i++) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBindTexture(GL_TEXTURE_2D, boxSurface);
			model = glm::mat4(1.0f);
			model = glm::translate(model, boxLocations[i]);
			boxShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		//modelShader Setup for lighting
		modelShader.useProgram();

		modelShader.setVec3("viewPos", currentCamera.cameraPos);

		modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		
		modelShader.setVec3("pointLights[0].ambient", 0.05f, 0.05, 0.05f);
		modelShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("pointLights[0].position", lightPos);
		modelShader.setFloat("pointLights[0].constant", 1.0f);
		modelShader.setFloat("pointLights[0].linear", 0.09f);
		modelShader.setFloat("pointLights[0].quadratic", 0.032f);

		modelShader.setVec3("spotlight.ambient", 0.0f, 0.0f, 0.0f);
		modelShader.setVec3("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("spotlight.specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("spotlight.position", currentCamera.cameraPos);
		modelShader.setVec3("spotlight.direction", currentCamera.front);
		modelShader.setFloat("spotlight.constant", 1.0f);
		modelShader.setFloat("spotlight.linear", 0.09f);
		modelShader.setFloat("spotlight.quadratic", 0.032f);
		modelShader.setFloat("spotlight.cutoff", glm::cos(glm::radians(12.5f)));
		modelShader.setFloat("spotlight.outerCutoff", glm::cos(glm::radians(15.0f)));


		//Nano suit
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("model", model);

		ourModel.Draw(modelShader);


		//Bounder
		/*glDisable(GL_DEPTH_TEST);
		bounderShader.useProgram();

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.80f, 0.0f));
		model = glm::scale(model, glm::vec3(0.205f, 0.205f, 0.205f));
		bounderShader.setMat4("view", view);
		bounderShader.setMat4("projection", projection);
		bounderShader.setMat4("model", model);

		ourModel.Draw(bounderShader);

		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glStencilMask(0xFF);*/

		//grass
		glBindVertexArray(planeVAO);
		discardShader.useProgram();
		discardShader.setMat4("view", view);
		discardShader.setMat4("projection", projection);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBindTexture(GL_TEXTURE_2D, grassSurface);
		for (int i = 0; i < boxLocations.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, boxLocations[i]);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.51f));
			discardShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);

		//window
		std::vector<glm::vec3> windowLocations;
		windowLocations.push_back(glm::vec3(0.1f, -0.5f, 4.0f));
		windowLocations.push_back(glm::vec3(0.9f, -0.8f, 5.0f));
		windowLocations.push_back(glm::vec3(0.5f, -1.1f, 6.0f));

		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < windowLocations.size(); i++) {
			float distance = glm::length(currentCamera.cameraPos - windowLocations[i]);
			sorted[distance] = windowLocations[i];
		}

		glBindVertexArray(planeVAO);
		imageShader.useProgram();
		imageShader.setMat4("view", view);
		imageShader.setMat4("projection", projection);
		glBindTexture(GL_TEXTURE_2D, windowSurface);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			imageShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);

		//Framebuffer Stage 2
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.useProgram();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		//Swap
		glfwSwapBuffers(window);	//Swap the double buffer
		glfwPollEvents();			//Poll IO events
	}

	//---------------------Optional: De-allocate all resources---------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &quadVBO);
	//Clear Resources
	glfwTerminate();
	return 0;
}

//---------------------------------------------------------------Main Function End---------------------------------------------------------------

//---------------------------------------------------------------Function Definition---------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

//Receive the input
void processInput(GLFWwindow* window) {
	//Exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		currentCamera.processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		currentCamera.processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		currentCamera.processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		currentCamera.processKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		cursorLastX = xPos;
		cursorLastY = yPos;
		firstMouse = false;
	}
	float xOffset = xPos - cursorLastX;
	float yOffset = cursorLastY - yPos;
	cursorLastX = xPos;
	cursorLastY = yPos;
	currentCamera.processMouseMovement(xOffset, yOffset, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	currentCamera.processMouseScroll(xOffset, yOffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = 0;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


















//For no Command Line Window,
//
//Expand the Linker options.
//Click on System.
//Change "SubSystem" to    Windows(/ SUBSYSTEM:WINDOWS)
//Click on Advanced under the expanded Linker items.
//In the Entry Point box, change it to mainCRTStartup.
//
//Change it back to console mode, Change "SubSystem" to      Console (/SUBSYSTEM:CONSOLE)