// David Moroni Sanchez Galvan
// Practica 8
// Fecha de entrega : 17/10/2025
// Numero de cuenta: 318107680

// Std. Includes
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp> // Para el color del cielo

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;


// --- Atributos de Luz (Sol y Luna) ---
float cycleAngle = 0.0f; // Ángulo del ciclo día/noche
float cycleRadius = 15.0f; // Radio del círculo imaginario
glm::vec3 cycleCenter(0.0f, 1.0f, 0.0f); // Centro de la rotación

// Propiedades del Sol (Intenso, amarillo-blanco)
glm::vec3 sunColor(1.0f, 1.0f, 0.8f);
glm::vec3 sunAmbient = sunColor * 0.4f;
glm::vec3 sunDiffuse = sunColor * 0.8f;
glm::vec3 sunSpecular = sunColor * 1.0f;

// Propiedades de la Luna (Tenue, azulada)
glm::vec3 moonColor(0.2f, 0.3f, 0.5f);
glm::vec3 moonAmbient = moonColor * 0.1f;
glm::vec3 moonDiffuse = moonColor * 0.2f;
glm::vec3 moonSpecular = moonColor * 0.05f;
// --- Fin de Atributos de Luz ---

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float rot = 0.0f;
bool activanim = false;

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "David Moroni Sanchez Galvan", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // Carga de todos los modelos
    Model dog((char*)"Models/RedDog.obj");
    Model pared((char*)"Models/pared.obj");
    Model baño((char*)"Models/baño.obj");
    Model sink((char*)"Models/Bathroom_Sink.obj");
    Model piso((char*)"Models/piso.obj");
    Model shelf((char*)"Models/shelf.obj");

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // --- VAO/VBO de la Lámpara 
    float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,-0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f,-0.5f, -0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,-0.5f, -0.5f, -0.5f,-0.5f, -0.5f, -0.5f,-0.5f, -0.5f,  0.5f,-0.5f,  0.5f,  0.5f,
       0.5f,  0.5f,  0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f,-0.5f, -0.5f,  0.5f,-0.5f, -0.5f, -0.5f,
      -0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    // --- Fin de Configuración de la Lámpara ---


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        // --- Color de Cielo Dinámico 
        float sunY_normalized = sin(cycleAngle);
        glm::vec3 skyColor = glm::mix(glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.5f, 0.7f, 1.0f), glm::max(0.0f, sunY_normalized * 1.5f));
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // --- Cálculo de Posiciones de Luz 
        glm::vec3 sunPos;
        sunPos.x = cycleCenter.x;
        sunPos.y = cycleCenter.y + sin(cycleAngle) * cycleRadius;
        sunPos.z = cycleCenter.z + cos(cycleAngle) * cycleRadius;

        glm::vec3 moonPos;
        moonPos.x = cycleCenter.x;
        moonPos.y = cycleCenter.y - sin(cycleAngle) * cycleRadius;
        moonPos.z = cycleCenter.z - cos(cycleAngle) * cycleRadius;


        // --- Renderizado en UNA SOLA PASADA ---
        lightingShader.Use();

        // --- MODIFICACIÓN ---
        // Enviar datos del SOL al shader
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "sunLight.position"), 1, &sunPos[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "sunLight.ambient"), 1, &sunAmbient[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "sunLight.diffuse"), 1, &sunDiffuse[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "sunLight.specular"), 1, &sunSpecular[0]);

        // Enviar datos de la LUNA al shader
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "moonLight.position"), 1, &moonPos[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "moonLight.ambient"), 1, &moonAmbient[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "moonLight.diffuse"), 1, &moonDiffuse[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.Program, "moonLight.specular"), 1, &moonSpecular[0]);
        // --- FIN MODIFICACIÓN ---

        // Uniforms comunes (Cámara y Material)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0.7f, 0.2f, 0.4f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 0.6f, 0.6f, 0.06f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 0.6f);

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // --- Dibujar todos los modelos 
        // Perro
        glm::mat4 modelDog = glm::mat4(1.0f);
        modelDog = glm::translate(modelDog, glm::vec3(0.53f, 0.0f, 1.0f));
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
        baño.Draw(lightingShader);

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


        // --- Renderizado de las Lámparas (Sol y Luna) ---
        lampShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);

        // Dibujar Lámpara SOL
        glm::mat4 modelLampSun = glm::mat4(1.0f);
        modelLampSun = glm::translate(modelLampSun, sunPos);
        modelLampSun = glm::scale(modelLampSun, glm::vec3(0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelLampSun));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Dibujar Lámpara LUNA
        glm::mat4 modelLampMoon = glm::mat4(1.0f);
        modelLampMoon = glm::translate(modelLampMoon, moonPos);
        modelLampMoon = glm::scale(modelLampMoon, glm::vec3(0.3f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelLampMoon));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        // --- Fin de Renderizado de Lámpara ---

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// --- Funciones de Callback 

void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    // Controles del Ciclo Día/Noche
    if (keys[GLFW_KEY_O])
    {
        cycleAngle += 0.02f; // Mover el ciclo
    }

    if (keys[GLFW_KEY_L])
    {
        cycleAngle -= 0.02f; // Mover el ciclo en reversa
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
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}