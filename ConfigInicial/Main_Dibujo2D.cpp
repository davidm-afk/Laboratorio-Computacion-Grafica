//David Moroni Sánchez Galván
//Practica 2
//Fecha de entrega: 21 de Agosto del 2025
//Número de cuenta: 318107680
#include<iostream>

//#define GLEW_STATIC

#include <GL/glew.h>

#include <GLFW/glfw3.h>

// Shaders
#include "Shader.h"

void resize(GLFWwindow* window, int width, int height);

const GLint WIDTH = 1000, HEIGHT = 1000;


int main() {
	glfwInit();
	//Verificaci�n de compatibilidad 
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Sanchez Galvan David", NULL, NULL);
	glfwSetFramebufferSizeCallback(window, resize);
	
	//Verificaci�n de errores de creacion  ventana
	if (window== NULL) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	//Verificaci�n de errores de inicializaci�n de glew

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Imprimimos informacin de OpenGL del sistema
	std::cout << "> Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "> Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "> Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "> SL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


	// Define las dimensiones del viewport
	//glViewport(0, 0, screenWidth, screenHeight);

    Shader ourShader("Shader/core.vs", "Shader/core.frag");

	ourShader.Use();
	glUniform1f(glGetUniformLocation(ourShader.Program, "uScale"), 0.8333333f); // 1/1.2


	// Set up vertex data (and buffer(s)) and attribute pointers
	float vertices[] = {
		0.0f,  0.0f, 0.0f,    1.0f,1.0f,0.0f,  //0
		0.05f,  -0.05f, 0.0f,    1.0f,1.0f,0.0f,  //1
		0.1f, -0.1f, 0.0f,    1.0f,1.0f,0.0f,  //2
		0.2f, -0.1f, 0.0f,    1.0f,1.0f,0.0f,  //3
		0.3f, -0.2f, 0.0f,    1.0f,1.0f,0.0f,  //4
		0.4f, -0.2f, 0.0f,    1.0f,1.0f,0.0f,  //5
		0.5f, -0.3f, 0.0f,    1.0f,1.0f,0.0f,  //6
		0.6f, -0.3f, 0.0f,    1.0f,1.0f,0.0f,  //7
		0.7f, -0.4f, 0.0f,    1.0f,1.0f,0.0f,  //8
		0.8f, -0.4f, 0.0f,    1.0f,1.0f,0.0f,  //9
		0.9f, -0.5f, 0.0f,    1.0f,1.0f,0.0f,  //10
		0.8f, -0.6f, 0.0f,    1.0f,1.0f,0.0f,  //11
		0.7f, -0.7f, 0.0f,    1.0f,1.0f,0.0f,  //12
		0.6f, -0.7f, 0.0f,    1.0f,1.0f,0.0f,  //13
		0.5f, -0.8f, 0.0f,    1.0f,1.0f,0.0f,  //14
		0.4f, -0.8f, 0.0f,    1.0f,1.0f,0.0f,  //15
		0.3f, -0.9f, 0.0f,    1.0f,1.0f,0.0f,  //16
		0.2f, -0.9f, 0.0f,    1.0f,1.0f,0.0f,  //17
		0.1f, -0.9f, 0.0f,    1.0f,1.0f,0.0f,  //18
		0.0f, -0.9f, 0.0f,    1.0f,1.0f,0.0f,  //19
		-0.1f, -0.9f, 0.0f,    1.0f,1.0f,0.0f,  //20
		-0.2f, -0.9f, 0.0f,    1.0f,1.0f,0.0f,  //21
		-0.3f, -0.8f, 0.0f,    1.0f,1.0f,0.0f,  //22
		-0.4f, -0.8f, 0.0f,    1.0f,1.0f,0.0f,  //23
		-0.5f, -0.7f, 0.0f,    1.0f,1.0f,0.0f,  //24
		-0.6f, -0.7f, 0.0f,    1.0f,1.0f,0.0f,  //25
		-0.7f, -0.6f, 0.0f,    1.0f,1.0f,0.0f,  //26
		-0.8f, -0.5f, 0.0f,    1.0f,1.0f,0.0f,  //27
		-0.8f, -0.4f, 0.0f,    1.0f,1.0f,0.0f,  //28
		-0.9f, -0.3f, 0.0f,    1.0f,1.0f,0.0f,  //29
		-0.9f, -0.2f, 0.0f,    1.0f,1.0f,0.0f,  //30
		-1.0f, -0.1f, 0.0f,    1.0f,1.0f,0.0f,  //31
		-1.0f, 0.0f, 0.0f,    1.0f,1.0f,0.0f,  //32
		-1.0f, 0.1f, 0.0f,    1.0f,1.0f,0.0f,  //33
		-1.0f,  0.2f, 0.0f,    1.0f,1.0f,0.0f,  //34
		-1.0f,  0.3f, 0.0f,    1.0f,1.0f,0.0f,  //35
		-1.0f,  0.4f, 0.0f,    1.0f,1.0f,0.0f,  //36
		-0.9f,  0.5f, 0.0f,    1.0f,1.0f,0.0f,  //37
		-0.9f,  0.6f, 0.0f,    1.0f,1.0f,0.0f,  //38
		-0.8f,  0.7f, 0.0f,    1.0f,1.0f,0.0f,  //39
		-0.8f,  0.8f, 0.0f,    1.0f,1.0f,0.0f,  //40
		-0.7f,  0.9f, 0.0f,    1.0f,1.0f,0.0f,  //41
		-0.6f,  1.0f, 0.0f,    1.0f,1.0f,0.0f,  //42
		-0.5f,  1.0f, 0.0f,    1.0f,1.0f,0.0f,  //43
		-0.4f,  1.1f, 0.0f,    1.0f,1.0f,0.0f,  //44
		-0.3f,  1.1f, 0.0f,    1.0f,1.0f,0.0f,  //45
		-0.2f,  1.2f, 0.0f,    1.0f,1.0f,0.0f,  //46
		-0.1f,  1.2f, 0.0f,    1.0f,1.0f,0.0f,  //47
		0.0f,  1.2f, 0.0f,    1.0f,1.0f,0.0f,  //48
		0.1f,  1.2f, 0.0f,    1.0f,1.0f,0.0f,  //49
		0.2f,  1.2f, 0.0f,    1.0f,1.0f,0.0f,  //50
		0.3f,  1.2f, 0.0f,    1.0f,1.0f,0.0f,  //51
		0.4f,  1.1f, 0.0f,    1.0f,1.0f,0.0f,  //52
		0.5f,  1.1f, 0.0f,    1.0f,1.0f,0.0f,  //53
		0.6f,  1.0f, 0.0f,    1.0f,1.0f,0.0f,  //54
		0.7f,  1.0f, 0.0f,    1.0f,1.0f,0.0f,  //55
		0.8f,  0.9f, 0.0f,    1.0f,1.0f,0.0f,  //56
		0.9f,  0.8f, 0.0f,    1.0f,1.0f,0.0f,  //57
		0.9f,  0.7f, 0.0f,    1.0f,1.0f,0.0f,  //58
		1.0f,  0.6f, 0.0f,    1.0f,1.0f,0.0f,  //59
		1.0f,  0.5f, 0.0f,    1.0f,1.0f,0.0f,  //60
		0.9f,  0.4f, 0.0f,    1.0f,1.0f,0.0f,  //61
		0.8f,  0.4f, 0.0f,    1.0f,1.0f,0.0f,  //62
		0.7f,  0.3f, 0.0f,    1.0f,1.0f,0.0f,  //63
		0.6f,  0.3f, 0.0f,    1.0f,1.0f,0.0f,  //64
		0.5f,  0.2f, 0.0f,    1.0f,1.0f,0.0f,  //65
		0.4f,  0.2f, 0.0f,    1.0f,1.0f,0.0f,  //66
		0.3f,  0.1f, 0.0f,    1.0f,1.0f,0.0f,  //67
		0.2f,  0.1f, 0.0f,    1.0f,1.0f,0.0f,  //68
		0.1f,  0.1f, 0.0f,    1.0f,1.0f,0.0f,  //69


	};
	unsigned int indices[] = {  // note that we start from 0!
		3,2,1,// second Triangle
		0,1,3,
		
	};



	GLuint VBO, VAO,EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Enlazar  Vertex Array Object
	glBindVertexArray(VAO);

	//2.- Copiamos nuestros arreglo de vertices en un buffer de vertices para que OpenGL lo use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3.Copiamos nuestro arreglo de indices en  un elemento del buffer para que OpenGL lo use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 4. Despues colocamos las caracteristicas de los vertices

	//Posicion
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


	
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// Draw our first triangle
        ourShader.Use();
        glBindVertexArray(VAO);


        glPointSize(10);
        glDrawArrays(GL_POINTS,0,70);
        
        //glDrawArrays(GL_LINES,0,2);

		glLineWidth(10);
        glDrawArrays(GL_LINE_LOOP,0,70);
        
        //glDrawArrays(GL_TRIANGLES,0,3);
        //glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,0);

        
        
        glBindVertexArray(0);
    
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}



	glfwTerminate();
	return EXIT_SUCCESS;
}

void resize(GLFWwindow* window, int width, int height)
{
	// Set the Viewport to the size of the created window
	glViewport(0, 0, width, height);
	//glViewport(0, 0, screenWidth, screenHeight);
}