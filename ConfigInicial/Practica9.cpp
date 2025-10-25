// David Moroni Sanchez Galvan
// Practica 9
// Fecha de entrega : 24/10/2025
// Numero de cuenta: 318107680

#include <iostream>
#include <cmath>
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 1.5f, 5.0f)); // Posición inicial ajustada para ver la escena
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
bool active = false;

// --- POSICIONES DE LUZ ---

// --- 1. Luz Direccional (Sol) ---
// No tiene posición, solo dirección. Pondremos un modelo visual lejos.
glm::vec3 sunModelPos(30.0f, 50.0f, 0.0f); // Posición visual del *modelo* del sol
glm::vec3 dirLightDirection(-0.2f, -1.0f, -0.3f); // Dirección real de la luz

// --- 2. Luces de Punto (Lámparas) ---
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f, 1.0f, 1.0f), // Lámpara 1 (la que movemos)
	glm::vec3(2.0f, 1.5f, 0.5f), // Lámpara 2 (apagada por ahora)
	glm::vec3(-1.5f, 2.0f, -1.0f), // Lámpara 3 (apagada por ahora)
	glm::vec3(0.0f, 1.0f, -2.0f)  // Lámpara 4 (apagada por ahora)
};

// --- 3. Reflector (Linterna) ---
// La despegamos de la cámara para poder *ver* el modelo de la linterna
glm::vec3 spotLightPos(0.0f, 2.5f, 3.0f);
glm::vec3 spotLightDir(0.0f, -0.5f, -1.0f); // Apuntando hacia abajo y al frente


glm::vec3 Light1 = glm::vec3(0);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Sanchez Galvan David Moroni", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST); // Habilitar test de profundidad


	// --- Shaders ---
	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");


	// --- Carga de modelos de la Escena (de Practica8.cpp) ---
	Model dog((char*)"Models/RedDog.obj");
	Model pared((char*)"Models/pared.obj");
	Model bano((char*)"Models/baño.obj");
	Model sink((char*)"Models/Bathroom_Sink.obj");
	Model piso((char*)"Models/piso.obj");
	Model shelf((char*)"Models/shelf.obj");

	// --- Carga de modelos para las Luces (NUEVO) ---
	// (Asegúrate de tener estos archivos .obj en tu carpeta Models)
	Model sunModel((char*)"Models/sun.obj");
	Model lampModel((char*)"Models/lamp.obj");
	Model flashlightModel((char*)"Models/flashlight.obj");


	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// --- Configuración de las 3 Luces (de fuentesDeLuz.cpp) ---

		// 1. Directional light
		glUniform3fv(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 1, &dirLightDirection[0]);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.1f, 0.1f, 0.1f); // Un poco más brillante
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);


		// 2. Point light 1 (Luz animada)
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x * 0.1f, lightColor.y * 0.1f, lightColor.z * 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// Point light 2 (Apagada)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0f);

		// Point light 3 (Apagada)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0f);

		// Point light 4 (Apagada)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0f);


		// 3. SpotLight (Linterna estática)
		// --- MODIFICADO --- Ya no sigue a la cámara
		glUniform3fv(glGetUniformLocation(lightingShader.Program, "spotLight.position"), 1, &spotLightPos[0]);
		glUniform3fv(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), 1, &spotLightDir[0]);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.075f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// --- Dibujar la Escena del Baño (de Practica8.cpp) ---
		glm::mat4 model(1);

		// Perro
		glm::mat4 modelDog = glm::mat4(1.0f);
		modelDog = glm::translate(modelDog, glm::vec3(0.53f, 0.0f, 1.0f));
		modelDog = glm::scale(modelDog, glm::vec3(0.5f)); // Un poco más pequeño
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelDog));
		dog.Draw(lightingShader);

		// Pared
		glm::mat4 modelPared = glm::mat4(1.0f);
		modelPared = glm::translate(modelPared, glm::vec3(-2.0f, -1.0f, 0.0f));
		modelPared = glm::scale(modelPared, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelPared));
		pared.Draw(lightingShader);

		// Baño
		glm::mat4 modelBano = glm::mat4(1.0f);
		modelBano = glm::scale(modelBano, glm::vec3(0.05f, 0.05f, 0.05f));
		modelBano = glm::translate(modelBano, glm::vec3(15.0f, 0.0f, 16.0f));
		modelBano = glm::rotate(modelBano, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelBano));
		bano.Draw(lightingShader);

		// Mirror (Sink)
		glm::mat4 modelSink = glm::mat4(1.0f);
		modelSink = glm::scale(modelSink, glm::vec3(0.021f, 0.021f, 0.021f));
		modelSink = glm::translate(modelSink, glm::vec3(100.0f, -10.0f, 22.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSink));
		sink.Draw(lightingShader);

		//Piso
		glm::mat4 modelPiso = glm::mat4(1.0f);
		modelPiso = glm::rotate(modelPiso, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPiso = glm::scale(modelPiso, glm::vec3(0.002f, 0.002f, 0.002f));
		modelPiso = glm::translate(modelPiso, glm::vec3(700.0f, -100.0f, 500.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelPiso));
		piso.Draw(lightingShader);

		//Shelf
		glm::mat4 modelShelf = glm::mat4(1.0f);
		modelShelf = glm::translate(modelShelf, glm::vec3(2.0f, 1.2f, 0.1f));
		modelShelf = glm::scale(modelShelf, glm::vec3(2.0f, 2.0f, 2.0f));
		modelShelf = glm::rotate(modelShelf, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelShelf));
		shelf.Draw(lightingShader);

		glBindVertexArray(0);


		// --- Dibujar los modelos de las Luces (NUEVO) ---
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// 1. Dibujar el Sol (Luz Direccional)
		model = glm::mat4(1);
		model = glm::translate(model, sunModelPos);
		model = glm::scale(model, glm::vec3(5.0f)); // Grande y lejano
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		sunModel.Draw(lampShader); // Usa el shader de lámpara (sin iluminación)

		// 2. Dibujar las Lámparas (Luces de Punto)
		for (GLuint i = 0; i < 1; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f)); // Ajusta esta escala al tamaño de tu modelo
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			lampModel.Draw(lampShader);
		}

		// 3. Dibujar la Linterna (Reflector)
		model = glm::mat4(1);
		model = glm::translate(model, spotLightPos);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.05f)); 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		flashlightModel.Draw(lampShader);


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// --- Controles de la Luz de Punto 0 ---
	if (keys[GLFW_KEY_R])
		pointLightPositions[0].x += 0.05f;
	if (keys[GLFW_KEY_F])
		pointLightPositions[0].x -= 0.05f;
	if (keys[GLFW_KEY_T])
		pointLightPositions[0].y += 0.05f;
	if (keys[GLFW_KEY_G])
		pointLightPositions[0].y -= 0.05f;
	if (keys[GLFW_KEY_Y])
		pointLightPositions[0].z -= 0.05f;
	if (keys[GLFW_KEY_H])
		pointLightPositions[0].z += 0.05f;

	// --- Controles del Reflector (Linterna) con Teclado Numérico ---
	if (keys[GLFW_KEY_U]) 
		spotLightPos.y += 0.05f;
	if (keys[GLFW_KEY_J]) 
		spotLightPos.y -= 0.05f;
	if (keys[GLFW_KEY_I]) 
		spotLightPos.x -= 0.05f;
	if (keys[GLFW_KEY_K]) 
		spotLightPos.x += 0.05f;
	if (keys[GLFW_KEY_O]) 
		spotLightPos.z += 0.05f;
	if (keys[GLFW_KEY_L]) 
		spotLightPos.z -= 0.05f;

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) 
		{
			keys[key] = false;
		}
	}

	// Se activa al presionar (no al mantener)
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		active = !active;
		if (active)
			Light1 = glm::vec3(1.0f, 0.5f, 0.2f); // Color de la animación
		else
			Light1 = glm::vec3(0);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}