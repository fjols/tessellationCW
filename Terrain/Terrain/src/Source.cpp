#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"

#include <Model.h>
#include "Terrain.h"

#include<string>
#include <iostream>
#include <numeric>



// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;
GLfloat RED = 1.0f;
GLfloat GREEN = 1.0f;
GLfloat BLUE = 1.0f;
glm::vec3 dirLightPos(0.1f, 0.6f, 0.2f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void setFBOcolour();
unsigned int loadTexture(char const * path);
//unsigned int loadTexture2(char const * path);
void setVAO(vector <float> vertices);
void setRBO();
void setFBOdepth();
void renderQuad();

// camera
Camera camera(glm::vec3(260,50,300));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int VBO, VAO, quadVBO_, FBO;
unsigned int textureColourBuffer;
unsigned int textureDepthBuffer;
unsigned int quadVAO = 0, quadVBO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float height = 100.0f;



int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	float texCoords[] = {
		uv1.x, uv1.y,
		uv2.x, uv2.y,
		uv3.x, uv3.y,
		uv1.x, uv1.y,
		uv3.x, uv3.y,
		uv4.x, uv4.y
	};

	// simple vertex and fragment shader - add your own tess and geo shader
	Shader shader("..\\shaders\\heightV.vs", "..\\shaders\\heightF.fs", "..\\shaders\\heightG.gs", "..\\shaders\\heightTC.tcs", "..\\shaders\\heightTE.tes");
	//Shader depthShader("..\\shaders\\depth.vs", "..\\shaders\\depthFrag.fs");
	Shader shadowMapShader("..\\shaders\\shadow.vs", "..\\shaders\\shadow.fs");

	const unsigned int shadowWidth = 1024, shadowHeight = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int heightMap = loadTexture("..\\resources\\heightMap.png");

	shader.use();
	//shader.setInt("texture1", 0);

	//Terrain Constructor ; number of grids in width, number of grids in height, gridSize
	Terrain terrain(100, 100,10);
	std::vector<float> vertices= terrain.getVertices();
	setVAO(vertices);


	//setFBOdepth();



	//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	shader.setInt("shadowMap", 0);


	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		float near_plane = 0.1f, far_plane = 1000.0f, orthSize = 250.0f;

		dirLightPos.y = 400.0f;
		dirLightPos.z = 500.0f;
		lightProjection = glm::ortho(-orthSize, orthSize, -orthSize, orthSize, near_plane, far_plane);
		lightView = glm::lookAt(dirLightPos, glm::vec3(250.0f, 125.0f, 250.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(700, 700, 724, 724);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//depthShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureDepthBuffer);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		renderQuad();

		glBindTexture(GL_TEXTURE_2D, heightMap);
		glActiveTexture(GL_TEXTURE1);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		shader.use();
		glEnable(GL_DEPTH_TEST);
		glClearColor(RED, GREEN, BLUE, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(VAO);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
	    shader.use();
	    shader.setMat4("projection", projection); // Projection matrix.
		shader.setMat4("view", view); // View matrix.
		shader.setMat4("model", model); // Model matrix.
		shader.setVec3("viewPos", camera.Position); // Position of the camera.
		shader.setVec3("eyePos", camera.Position); // Position of the camera.
		shader.setFloat("heightFactor", height); // Height factor.

		glm::vec3 matColour;
		matColour.x = sin(glfwGetTime() * 2.0f);
		matColour.y = sin(glfwGetTime() * 0.7f);
		matColour.z = sin(glfwGetTime() * 1.3f);

		glm::vec3 diffuseColour = matColour * glm::vec3(0.9f);
		glm::vec3 ambientColour = matColour * glm::vec3(0.5f);

		//light properties
		shader.setVec3("dirLight.direction", dirLightPos); // Direction of the light.
		shader.setVec3("dirLight.ambient", 1.0f, 1.0f, 1.0f); // Ambient lighting.
		shader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f); // Diffuse lighting.
		shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f); // Specular lighting.
		//material properties
		shader.setVec3("mat.ambient", 1.0f, 1.0, 1.0f); // Ambient colour of the material.
		shader.setVec3("mat.diffuse", 1.0f, 1.0f, 1.0f); // Diffuse colour of the material.
		shader.setVec3("mat.specular", 1.0f, 1.0f, 1.0f); // Specular colour of the material.
		shader.setFloat("mat.shininess", 0.85f); // Shininess of the material.
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// Shadows

		shadowMapShader.use();
		shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shadowMapShader.setMat4("model", model);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, shadowWidth, shadowHeight);
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

		// Render scene
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			shader.setBool("gammaCorrection", true);
		}
		else
		{
			shader.setBool("gammaCorrection", false);
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // If the L button is pressed, change polygon mode to GL_FILL from GL_LINE.
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0, vertices.size() / 3);
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) 
		  camera.printCameraCoords();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		height += 1.f;
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
		height -= 1.f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void setFBOcolour()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glGenTextures(1, &textureColourBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, textureColourBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColourBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void setFBOdepth()
{
	glGenFramebuffers(1, &FBO);
	//Depth texture.
	glGenTextures(1, &textureDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, textureDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_HEIGHT, SCR_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	// Texture parameters for sampling and repeating.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Attach depth texture as FBO depth buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_TEXTURE_2D, textureDepthBuffer, 0);
	// Using GL_NONE as not using colour buffer.
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setRBO()
{
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
		std::cout << "Loaded texture at path: " << path << " width " << width << " id " << textureID <<  std::endl;

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		
	}

	return textureID;
}


void setVAO(vector <float> vertices) {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);		
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), vertices.data(), GL_STATIC_DRAW);

	//xyz
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



