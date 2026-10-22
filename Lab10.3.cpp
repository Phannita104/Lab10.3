
// lab10.3.cpp
// Computer Graphics: Lab10.03 - Purple ADS Pyramid

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Computer Graphics: Lab10.03 - Purple Pyramid",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    Shader lightingShader("colors.vs", "colors.fs");
    Shader lightCubeShader("cube.vs", "cube.fs");

    // Pyramid vertices: Position (x,y,z) + Normal (x,y,z)
    float vertices[] =
    {
        // Front face
         0.0f,  0.5f,  0.0f,    0.0f,  0.4472f,  0.8944f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.4472f,  0.8944f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.4472f,  0.8944f,

         // Right face
          0.0f,  0.5f,  0.0f,    0.8944f,  0.4472f,  0.0f,
          0.5f, -0.5f,  0.5f,    0.8944f,  0.4472f,  0.0f,
          0.5f, -0.5f, -0.5f,   0.8944f,  0.4472f,  0.0f,

          // Back face
           0.0f,  0.5f,  0.0f,    0.0f,  0.4472f, -0.8944f,
           0.5f, -0.5f, -0.5f,    0.0f,  0.4472f, -0.8944f,
          -0.5f, -0.5f, -0.5f,    0.0f,  0.4472f, -0.8944f,

          // Left face
           0.0f,  0.5f,  0.0f,   -0.8944f,  0.4472f,  0.0f,
          -0.5f, -0.5f, -0.5f,   -0.8944f,  0.4472f,  0.0f,
          -0.5f, -0.5f,  0.5f,   -0.8944f,  0.4472f,  0.0f,

          // Base triangle 1
          -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
           0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
           0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,

           // Base triangle 2
            0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
           -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
           -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f
    };

    // Create VAO and VBO
    unsigned int VBO, pyramidVAO;

    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Position attribute
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // Light object VAO
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pyramid lighting
        lightingShader.use();

        // Purple color
        lightingShader.setVec3(
            "objectColor",
            0.7f,
            0.2f,
            1.0f
        );

        // Light color: LightSalmon
        lightingShader.setVec3(
            "lightColor",
            1.0f,
            0.627451f,
            0.478431f
        );

        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // Projection and view
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );

        glm::mat4 view = camera.GetViewMatrix();

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Pyramid model
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // Draw pyramid
        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // Draw light object
        lightCubeShader.use();

        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// Keyboard controls: W, S, A, D
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
}

// Window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
