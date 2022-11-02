/*
    Final Project
    Group: 12

    Members: Castro, Steven Jentyn
             Desembrana, Anna Patricia Bernardino
             Mandadero, Clarissa Mae Suavengco

    Section: S13
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

    Bunny bunny1 = Bunny(vec3(-0.5f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 45.0f, 2.0f));
    Bunny bunny2 = Bunny(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 90.0f, 2.0f));
    Bunny bunny3 = Bunny(vec3(0.5f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 135.0f, 2.0f));

    // Create Buffer Objects // // // // // // // // // // // // // // // // // // // // // // // //

    vector<GLfloat> bunnyVertexData = bunny1.loadVertexData();
    GLuint BunnyVAO = bunny1.setBuffers(bunnyVertexData);
    bunny1.setAttribPointer();

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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Update Object Properties
        bunny1.rotation.y = bunny1.rotation.y - 0.5;
        bunny2.rotation.y = bunny1.rotation.y + 0.5;
        bunny3.rotation.y = bunny1.rotation.y - 0.5;

        // Draw 
        bunny1.draw(SMBunnyA.shaderProgram, bunnyVertexData, BunnyVAO);
        bunny2.draw(SMBunnyB.shaderProgram, bunnyVertexData, BunnyVAO);
        bunny3.draw(SMBunnyA.shaderProgram, bunnyVertexData, BunnyVAO);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}