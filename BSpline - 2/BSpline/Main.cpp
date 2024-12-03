#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "Camera.h"
#include "BSplineSurface.h"
#include "Ball.h"
#include "Collision.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int SCR_WIDTH, int SCR_HEIGHT); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window); 

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(1.5f, 1.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float speedFactor = 0.0f;
float ballRadius = 0.05; // Radius til ballene

// Lys
glm::vec3 lightPos(10.0f, 10.0f, 20.0f);

// Konstant grenser
const float minX = 0.05f;
const float maxX = 2.95f;
const float minZ = -1.95f;
const float maxZ = -0.05f;

glm::vec3 input(float minX, float maxX, float minZ, float maxZ, float radius) {
	glm::vec3 position;
	while (true) {
		std::cout << "Skriv inn startkoordinater for ball (x z) innenfor følgende grenser:" << std::endl;
		std::cout << "x: [" << minX + radius << ", " << maxX - radius << "], z: [" << minZ + radius << ", " << maxZ - radius << "]" << std::endl;
		std::cout << "Format: x z" << std::endl;

		std::cin >> position.x >> position.z;

		if (position.x >= minX + radius && position.x <= maxX - radius &&
			position.z >= minZ + radius && position.z <= maxZ - radius) {
			position.y = radius;
			break;
		}
		else {
			std::cout << "Ugyldige koordinater. Prøv igjen." << std::endl;
		}
	}
	return position;
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "B-Spline", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGL();

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	Shader shaderProgram("default.vert", "default.frag");

	// Flaten
	BSplineSurface bsplineSurface;
	bsplineSurface.GenerateSurface(30, 30);

	// Ball
	Ball ball1(ballRadius, 36, 18, glm::vec3(0.8f, 0.0f, 0.0f)); // Rød
	Ball ball2(ballRadius, 36, 18, glm::vec3(0.0f, 0.0f, 0.8f)); // Blå
	Ball ball3(ballRadius, 36, 18, glm::vec3(0.0f, 0.8f, 0.0f)); // Grønn

	// Interaktivt input for startposisjon
	std::cout << "Velg startposisjon for ball 1:" << std::endl;
	ball1.position = input(minX, maxX, minZ, maxZ, ballRadius);

	std::cout << "Velg startposisjon for ball 2:" << std::endl;
	ball2.position = input(minX, maxX, minZ, maxZ, ballRadius);

	std::cout << "Velg startposisjon for ball 3:" << std::endl;
	ball3.position = input(minX, maxX, minZ, maxZ, ballRadius);


	ball1.velocity = glm::vec3(0.5f, 0.0f, 0.3f);
	ball2.velocity = glm::vec3(0.3f, 0.0f, -0.4f);
	ball3.velocity = glm::vec3(-0.2f, 0.0f, 0.1f); 

	glPointSize(5.0f);

	glEnable(GL_DEPTH_TEST);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// Ball bevegelse og kollisjon for at ballene ikke går utenfor flaten.
		ball1.position += ball1.velocity * deltaTime * speedFactor;
		Collision::checkWallCollision(ball1.position, ball1.velocity, 0.05f, 2.95f, -1.95f, -0.05f, ballRadius);
	
		ball2.position += ball2.velocity * deltaTime * speedFactor;
		Collision::checkWallCollision(ball2.position, ball2.velocity, 0.05f, 2.95f, -1.95f, -0.05f, ballRadius);

		ball3.position += ball3.velocity * deltaTime * speedFactor;
		Collision::checkWallCollision(ball3.position, ball3.velocity, 0.05f, 2.95f, -1.95f, -0.05f, ballRadius);

		// Ball-til-ball kollisjon
		Collision::responseBallCollision(ball1.position, ball2.position, ball1.velocity, ball2.velocity, ballRadius);
		Collision::responseBallCollision(ball1.position, ball3.position, ball1.velocity, ball3.velocity, ballRadius);
		Collision::responseBallCollision(ball2.position, ball3.position, ball2.velocity, ball3.velocity, ballRadius);

		// Render
		glClearColor(0.5f, 0.3f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();

		// Phong shader
		shaderProgram.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		shaderProgram.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shaderProgram.setVec3("lightPos", lightPos);
		shaderProgram.setVec3("viewPos", camera.Position);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); 
		shaderProgram.setMat4("projection", projection); 

		glm::mat4 view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.5f, 0.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); 
		shaderProgram.setMat4("model", model);
	
		// BSplineSurface
		bsplineSurface.DrawBSpline(shaderProgram);
		// Normalene til b-spline flaten
		bsplineSurface.DrawNormals(shaderProgram);

		//Ballene
		ball1.DrawBall(shaderProgram);
		ball2.DrawBall(shaderProgram);
		ball3.DrawBall(shaderProgram);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
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

	// Juster "speedfactor" med opp og ned pilene
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		speedFactor += 0.01f; // Øk farten
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		speedFactor = max(0.01f, speedFactor - 0.01f); // Minke farten
}

void framebuffer_size_callback(GLFWwindow* window, int SCR_WIDTH, int SCR_HEIGHT)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	std::cout << "Window resized with " << SCR_WIDTH << "Height" << SCR_HEIGHT << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouse)
	{
		lastX = static_cast<float>(xpos);   
		lastY = static_cast<float>(ypos);   
		firstMouse = false;
	}

	float xoffset = static_cast<float>(xpos) - lastX;
	float yoffset = lastY - static_cast<float>(ypos); 

	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	camera.ProcessMouseMovement(xoffset, yoffset);
}