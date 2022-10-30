/*

    Testing Grounds for OpenGL Code

*/

#include <string>
#include <iostream>
using namespace std;

#include <glad/glad.h>          
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#define TINYOBJLOADER_IMPLEMENTATION
#include "loaders/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "loaders/stb_image.h"

#include "class/header.h"

// Global Parameters & Settings // // // // // // // // // // // // // // // // // // // // // // // // 

float screen_width = 750.0f;
float screen_height = 750.0f;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screen_width, screen_height, "Final Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Screen Space
    glViewport(0, 0, screen_width, screen_height);

    // Load Object Models // // // // // // // // // // // // // // // // // // // // // // // //

    Bunny object = Bunny(vec3(-0.5f, 0.0f, 0.0f), 1.0f, vec3(0.0f, 45.0f, 2.0f));
    Bunny object2 = Bunny(vec3(0.0f, 0.0f, 0.0f), 1.0f, vec3(0.0f, 90.0f, 2.0f));
    Bunny object3 = Bunny(vec3(0.5f, 0.0f, 0.0f), 1.0f, vec3(0.0f, 135.0f, 2.0f));

    // Create Buffer Objects // // // // // // // // // // // // // // // // // // // // // // // //

    // Setup Vertex Array Object
    GLuint BunnyVAO;
    glGenVertexArrays(1, &BunnyVAO);
    glBindVertexArray(BunnyVAO);

    // Setup Vertex Buffer Object
    GLuint BunnyVBO;
    glGenBuffers(1, &BunnyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, BunnyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * object.fullVertexData.size(), object.fullVertexData.data(), GL_STATIC_DRAW);

    // Create Attrib Pointers // // // // // // // // // // // // // // // // // // // // // // // //

    // X Y Z
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setup Shaders // // // // // // // // // // // // // // // // // // // // // // // //

    ShaderManager SMBunnyA = ShaderManager();
    SMBunnyA.addVertexShader("Shaders/sample.vert");
    SMBunnyA.addFragmentShader("Shaders/sample.frag");
    SMBunnyA.link();

    ShaderManager SMBunnyB = ShaderManager();
    SMBunnyB.addVertexShader("Shaders/sample.vert");
    SMBunnyB.addFragmentShader("Shaders/sample2.frag");
    SMBunnyB.link();

    // Sky Box // // // // // // // // // // // // // // // // // // // // // // // //
    SkyBox skybox = SkyBox();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Update Object Properties
        object.rotation.y = object.rotation.y - 0.5;
        object2.rotation.y = object.rotation.y + 0.5;
        object3.rotation.y = object.rotation.y - 0.5;

        // Draw 
        object.draw(SMBunnyA.shaderProgram, BunnyVAO);
        object2.draw(SMBunnyB.shaderProgram, BunnyVAO);
        object3.draw(SMBunnyA.shaderProgram, BunnyVAO);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}