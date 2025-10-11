// David Moroni Sanchez Galvan
// Practica 7
// Fecha de entrega: 10/10/2025
// Numero de cuenta: 318107680

#include <iostream>
#include <cmath>

// ====== LIBRER�AS DE OPENGL ======
// GLEW: Gestiona las extensiones de OpenGL
#include <GL/glew.h>

// GLFW: Gestiona la ventana, el contexto de OpenGL y las entradas del usuario (teclado, mouse)
#include <GLFW/glfw3.h>

// STB_IMAGE: Librer�a para cargar im�genes (nuestra textura)
#include "stb_image.h"

// GLM: Librer�a para operaciones matem�ticas (vectores, matrices)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ====== ARCHIVOS DE CABECERA PROPIOS ======
#include "Shader.h" // Clase para compilar y usar los shaders
#include "Camera.h" // Clase para manejar la c�mara y su movimiento

// ====== PROTOTIPOS DE FUNCIONES ======
// Funci�n que se llama cada vez que se presiona/suelta una tecla
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Funci�n que se llama cada vez que se mueve el mouse
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
// Funci�n para procesar el movimiento de la c�mara
void DoMovement();

// ====== VARIABLES GLOBALES ======
// Dimensiones de la ventana
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// C�mara: se inicializa en la posici�n (0,0,3)
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// Posici�n inicial del mouse
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
// Arreglo para registrar qu� teclas est�n presionadas
bool keys[1024];
// Bandera para el primer movimiento del mouse
bool firstMouse = true;

// DeltaTime: tiempo entre el frame actual y el anterior. Para movimiento fluido.
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ====== FUNCI�N PRINCIPAL ======
int main()
{
	// --- 1. INICIALIZACI�N DE GLFW ---
	glfwInit();
	// Configuraci�n de la versi�n de OpenGL (3.3) y el perfil (Core)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // La ventana no se podr� redimensionar

	// Creaci�n de la ventana
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 7 - David Moroni Sanchez Galvan", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "Fallo al crear la ventana de GLFW" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	// Establece la ventana actual como el contexto principal para OpenGL
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// --- 2. ASIGNACI�N DE FUNCIONES CALLBACK ---
	// Asigna nuestras funciones para que se ejecuten cuando ocurran eventos de teclado y mouse
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// --- 3. CONFIGURACI�N DE ENTRADA ---
	// Oculta el cursor del mouse y lo captura para que no se salga de la ventana
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// --- 4. INICIALIZACI�N DE GLEW ---
	glewExperimental = GL_TRUE; // Necesario para el perfil Core
	if (GLEW_OK != glewInit())
	{
		std::cout << "Fallo al inicializar GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// --- 5. CONFIGURACI�N DE OPENGL ---
	// Define el �rea de la ventana donde se dibujar� (toda la ventana)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Habilita el test de profundidad para que los objetos se dibujen correctamente en 3D
	glEnable(GL_DEPTH_TEST);

	// --- 6. COMPILACI�N DE SHADERS ---
	// Crea y compila nuestro programa de shaders a partir de los archivos .vs y .frag
	Shader ourShader("Shader/lamp.vs", "Shader/lamp.frag");

	// --- 7. DEFINICI�N DE LA GEOMETR�A DEL CUBO ---
	// Cada l�nea es un v�rtice. Estructura: 3 floats para Posici�n (X,Y,Z), 3 para Color (R,G,B), 2 para Coordenadas de Textura (U,V)
	GLfloat vertices[] = {
		// Posiciones          // Color (Blanco) // Coordenadas de Textura (U, V)

		// Cara Trasera (Gris - 1)
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.25f, 1.0f / 3.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.00f, 1.0f / 3.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.00f, 2.0f / 3.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.00f, 2.0f / 3.0f,
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.25f, 2.0f / 3.0f,
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.25f, 1.0f / 3.0f,

		// Cara Frontal (P�rpura - 3)
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 1.0f / 3.0f,
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 1.0f / 3.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 2.0f / 3.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 2.0f / 3.0f,
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 2.0f / 3.0f,
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 1.0f / 3.0f,

		// Cara Izquierda (Rosa - 2)
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 2.0f / 3.0f,
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.25f, 2.0f / 3.0f,
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.25f, 1.0f / 3.0f,
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.25f, 1.0f / 3.0f,
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 1.0f / 3.0f,
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 2.0f / 3.0f,

		// Cara Derecha (Verde - 4)
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 2.0f / 3.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.00f, 2.0f / 3.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.00f, 1.0f / 3.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.00f, 1.0f / 3.0f,
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 1.0f / 3.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 2.0f / 3.0f,

		 // Cara Inferior (Rojo - 5)
		 -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 0.00f,
		  0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 0.00f,
		  0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 1.0f / 3.0f,
		  0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 1.0f / 3.0f,
		 -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 1.0f / 3.0f,
		 -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 0.00f,

		 // Cara Superior (Azul - 6)
		 -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 2.0f / 3.0f,
		  0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 2.0f / 3.0f,
		  0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 1.00f,
		  0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.75f, 1.00f,
		 -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 1.00f,
		 -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.50f, 2.0f / 3.0f
	};

	// --- 8. CONFIGURACI�N DE BUFFERS (VAO y VBO) ---
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO); // Genera un Vertex Array Object
	glGenBuffers(1, &VBO);      // Genera un Vertex Buffer Object

	glBindVertexArray(VAO); // Activa el VAO

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Activa el VBO
	// Copia los datos de nuestros v�rtices al VBO en la GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// --- 9. CONFIGURACI�N DE ATRIBUTOS DE V�RTICES ---
	// Le decimos a OpenGL c�mo interpretar los datos de los v�rtices en el VBO
	// Atributo de Posici�n (layout = 0 en el shader)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Atributo de Color (layout = 1 en el shader)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Atributo de Coordenada de Textura (layout = 2 en el shader)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Desactiva el VAO

	// --- 10. CARGA DE LA TEXTURA ---
	GLuint texture;
	glGenTextures(1, &texture);

	int textureWidth, textureHeight, nrChannels;
	// Invierte la imagen verticalmente al cargarla, porque OpenGL lee las texturas de abajo hacia arriba
	stbi_set_flip_vertically_on_load(true);
	// Carga la imagen del dado
	unsigned char* image = stbi_load("images/dado.png", &textureWidth, &textureHeight, &nrChannels, 0);

	// Activa la textura y establece sus par�metros
	glBindTexture(GL_TEXTURE_2D, texture);
	// Configuraci�n del "wrapping" (c�mo se repite la textura)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Configuraci�n del "filtering" (c�mo se ve la textura al escalar)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (image)
	{
		// Carga los datos de la imagen en la textura de OpenGL
		// Se usa GL_RGBA porque los PNG suelen tener canal de transparencia (Alpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		// Genera mipmaps para mejorar la calidad visual de la textura a diferentes distancias
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Fallo al cargar la textura" << std::endl;
	}
	// Libera la memoria de la imagen una vez cargada en la GPU
	stbi_image_free(image);

	// --- 11. BUCLE PRINCIPAL DEL JUEGO (GAME LOOP) ---
	while (!glfwWindowShouldClose(window))
	{
		// Calcula el deltaTime para este frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Revisa si hay eventos (teclado, mouse) y llama a las funciones correspondientes
		glfwPollEvents();
		DoMovement();

		// Limpia el buffer de color y el buffer de profundidad
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activa nuestro programa de shaders
		ourShader.Use();

		// --- 12. TRANSFORMACIONES (MATRICES) ---
		// Matriz de Vista (View): Mueve la escena en relaci�n a la c�mara
		glm::mat4 view = camera.GetViewMatrix();
		// Matriz de Proyecci�n (Projection): Define la perspectiva de la c�mara (FOV, etc.)
		glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		// Matriz de Modelo (Model): Posici�n, rotaci�n y escala del objeto en el mundo. Matriz identidad para un objeto est�tico en el origen.
		glm::mat4 model(1);

		// --- 13. ENV�O DE UNIFORMS A LOS SHADERS ---
		// Obtiene la ubicaci�n de las variables uniform (matrices) en el shader
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");

		// Env�a los datos de las matrices a los shaders
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// --- 14. DIBUJADO DEL OBJETO ---
		// Activa la unidad de textura 0
		glActiveTexture(GL_TEXTURE0);
		// Vincula nuestra textura del dado a la unidad de textura activa
		glBindTexture(GL_TEXTURE_2D, texture);

		// Activa el VAO del cubo
		glBindVertexArray(VAO);
		// Dibuja los 36 v�rtices como tri�ngulos
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Desactiva el VAO
		glBindVertexArray(0);

		// Intercambia los buffers de la pantalla (frontal y trasero) para mostrar lo que se ha renderizado
		glfwSwapBuffers(window);
	}

	// --- 15. LIMPIEZA DE RECURSOS ---
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Termina GLFW, liberando todos los recursos
	glfwTerminate();

	return 0;
}

// Implementaci�n de la funci�n para mover la c�mara con el teclado
void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Implementaci�n de la funci�n callback para el teclado
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Si se presiona la tecla ESCAPE, se cierra la ventana
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// Registra si una tecla est� presionada o ha sido soltada
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
}

// Implementaci�n de la funci�n callback para el mouse
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	// Si es el primer movimiento del mouse, se actualiza la posici�n inicial
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	// Calcula el desplazamiento del mouse desde el �ltimo frame
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Invertido porque las coordenadas Y van de abajo hacia arriba

	// Actualiza la posici�n del mouse para el siguiente frame
	lastX = xPos;
	lastY = yPos;

	// Procesa el movimiento del mouse para rotar la c�mara
	camera.ProcessMouseMovement(xOffset, yOffset);
}