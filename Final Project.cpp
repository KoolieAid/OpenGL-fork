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

float screenWidth = 750.0f;
float screenHeight = 750.0f;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Final Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glClearColor(0.493, 0.557, 0.880, 1.0f);

    // Screen Space
    glViewport(0, 0, screenWidth, screenHeight);


    // Load Object Models // // // // // // // // // // // // // // // // // // // // // // // //

    float bassScale = 1.0f / 8.0f;
    vector<Bass> basses;
    
    for (int i = 0; i < 10; i++) {
        basses.push_back(Bass(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(bassScale), vec3(0.0f, 90.0f, 2.0f)));
    }

    float sharkScale = 1.0f / 4.0;
    Shark shark = Shark(vec3(-50.0f, 0.0f, 10.0f), vec3(sharkScale), vec3(0.0, 0.0f, 2.0f));

    float whaleScale = 6.0f;
    Whale whale = Whale(vec3(-200.0f, 0.0f, 10.0f), vec3(whaleScale), vec3(0.0f, 0.0f, 0.0f));

    // Create Vertex Buffer Objects // // // // // // // // // // // // // // // // // // // // // // // //

    vector<GLfloat> bassVertexData = basses[0].loadVertexData();
    GLuint BassVAO = setBuffers(bassVertexData);
    basses[0].setAttribPointer();

    vector<GLfloat> sharkVertexData = shark.loadVertexData();
    GLuint SharkVAO = setBuffers(sharkVertexData);
    shark.setAttribPointer();

    vector<GLfloat> whaleVertexData = whale.loadVertexData();
    GLuint WhaleVAO = setBuffers(whaleVertexData);
    whale.setAttribPointer();

    // Vertex Sizes
    int bassSize = bassVertexData.size();
    int sharkSize = sharkVertexData.size();
    int whaleSize = whaleVertexData.size();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Create Texture Buffer Objects // // // // // // // // // // // // // // // // // // // // // // // //

    GLuint bassTex = basses[0].loadTextures();
    GLuint sharkTex = shark.loadTextures();
    GLuint whaleTex = whale.loadTextures();
    GLuint whaleNormTex = whale.loadNormalTextures();


    // Setup Shaders // // // // // // // // // // // // // // // // // // // // // // // //

    MyShader SMBass = MyShader("Shaders/bass.vert", "Shaders/bass.frag");
    MyShader SMShark = MyShader("Shaders/shark.vert", "Shaders/shark.frag");
    MyShader SMWhale = MyShader("Shaders/whale.vert", "Shaders/whale.frag");


    // Setup Camera (Temp Setup)

    MyCamera camera = MyCamera(vec3(5.0f, 0.0f, 10.0f), screenHeight, screenWidth);
    camera.center.z = 10.0f;


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Whale
        whale.draw(SMWhale, whaleSize, WhaleVAO, whaleTex, whaleNormTex, camera);
        whale.position.z = (whale.position.z > 400.0f) ? (-400.0f) : (whale.position.z + 0.20f);

        // Bass
        for (int i = 0; i < basses.size(); i++) {
            basses[i].draw(SMBass, bassSize, BassVAO, bassTex, camera);
            basses[i].position.z = fmod(basses[i].position.z, 20.0f) + (i / 100.0f);
        }

        // Shark
        shark.draw(SMShark, sharkSize, SharkVAO, sharkTex, camera);
        shark.position.z = (shark.position.z > 80.0f) ? (-80.0f) : (shark.position.z + 0.15f);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}