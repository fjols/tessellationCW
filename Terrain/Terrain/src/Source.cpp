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
glm::vec3 dirLightPos(0.1f, 0.6f, 0.2f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
//unsigned int loadTexture2(char const * path);
void setVAO(vector <float> vertices);

// camera
Camera camera(glm::vec3(260,50,300));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int VBO, VAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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

	unsigned int heightMap = loadTexture("..\\resources\\heightMap.png");

	shader.use();
	shader.setInt("texture1", 0);

	//Terrain Constructor ; number of grids in width, number of grids in height, gridSize
	Terrain terrain(50, 50,10);
	std::vector<float> vertices= terrain.getVertices();
	setVAO(vertices);

	


	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glBindTexture(GL_TEXTURE_2D, heightMap);
		glActiveTexture(GL_TEXTURE1);

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
		shader.setVec3("mat.ambient", 0.7f, 0.7f, 0.7f); // Ambient colour of the material.
		shader.setVec3("mat.diffuse", 0.7f, 0.7f, 0.7f); // Diffuse colour of the material.
		shader.setVec3("mat.specular", 0.7f, 0.7f, 0.7f); // Specular colour of the material.
		shader.setFloat("mat.shininess", 0.85f); // Shininess of the material.

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // If the L button is pressed, change polygon mode to GL_FILL from GL_LINE.
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



