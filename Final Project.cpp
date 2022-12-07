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

float screenWidth = 1920.0f;
float screenHeight = 1080.0f;

// Control Pointers
vec3* cPosition;
vec3* cScale;
vec3* cRotation;

// Setting
int intensity_level = 2;  

// Key Callbacks [Controls?/Debugging] // // // // // // // // // // // // // // // // // // // // // // // // 
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_W: cRotation->z += 1.0f;
        break;
    case GLFW_KEY_S: cRotation->z -= 1.0f;
        break;
    case GLFW_KEY_A: cRotation->x -= 1.0f;
        break;
    case GLFW_KEY_D: cRotation->x += 1.0f;
        break;
    case GLFW_KEY_Q: cRotation->y += 1.0f;
        break;
    case GLFW_KEY_E: cRotation->y -= 1.0f;
        break;
    case GLFW_KEY_SPACE:
        break;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        intensity_level++;
        if (intensity_level == 4) {
            intensity_level = 1;
        }

        cout << "Light Intensity: " << intensity_level << endl;
    }
}



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

    // Inputs
    glfwSetKeyCallback(window, keyCallback);


    // Load Object Models // // // // // // // // // // // // // // // // // // // // // // // //

    // Object Scales
    float bassScale = 1.0f / 16.0f;
    float sharkScale = 1.0f / 32.0;
    float whaleScale = 6.0f;
    float submarineScale = 1.0 / 16.0f;
    float blueBettaScale = 1.0f / 512.0f;
    float spadeFishScale = 1.0f / 512.0f;
    float troutScale = 1.0f / 32.0f;
    float angelFishScale = 1.0f / 4.0f;

    vector<Bass> basses;
    for (int i = 0; i < 12; i++) {
        basses.push_back(Bass(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(bassScale), vec3(0.0f, 90.0f, 2.0f)));
    }

    vector<BlueBetta> blueBettas;
    for (int i = 0; i < 12; i++) {
        blueBettas.push_back(BlueBetta(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(blueBettaScale), vec3(0.0f, 90.0f, 2.0f)));
    }

    vector<SpadeFish> spadeFishes;
    for (int i = 0; i < 8; i++) {
        spadeFishes.push_back(SpadeFish(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(spadeFishScale), vec3(0.0f, 90.0f, 2.0f)));
    }

    vector<AngelFish> angelFishes;
    for (int i = 0; i < 12; i++) {
        angelFishes.push_back(AngelFish(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(angelFishScale), vec3(0.0f, -90.0f, 2.0f)));
    }

    vector<Trout> trouts;
    for (int i = 0; i < 12; i++) {
        trouts.push_back(Trout(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(troutScale), vec3(0.0f, 0.0f, 2.0f)));
    }

    Shark shark = Shark(vec3(-50.0f, 0.0f, 10.0f), vec3(sharkScale), vec3(0.0, 0.0f, 2.0f));
    Whale whale = Whale(vec3(-200.0f, 0.0f, -100.0f), vec3(whaleScale), vec3(0.0f, 0.0f, 0.0f));
    Submarine submarine = Submarine(vec3(-60.0f, -10.0f, 10.0f), vec3(submarineScale), vec3(0.0f));


    // Create Vertex Buffer Objects // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "> Loading Vertex Data...\n";

    vector<GLfloat> bassVertexData = basses[0].loadVertexData();
    GLuint BassVAO = setBuffers(bassVertexData);
    basses[0].setAttribPointer();

    vector<GLfloat> sharkVertexData = shark.loadVertexData();
    GLuint SharkVAO = setBuffers(sharkVertexData);
    shark.setAttribPointer();

    vector<GLfloat> whaleVertexData = whale.loadVertexData();
    GLuint WhaleVAO = setBuffers(whaleVertexData);
    whale.setAttribPointer();

    vector<GLfloat> submarineVertexData = submarine.loadVertexData();
    GLuint SubmarineVAO = setBuffers(submarineVertexData);
    submarine.setAttribPointer();

    vector<GLfloat> blueBettaVertexData = blueBettas[0].loadVertexData();
    GLuint BlueBettaVAO = setBuffers(blueBettaVertexData);
    blueBettas[0].setAttribPointer();

    vector<GLfloat> spadeFishVertexData = spadeFishes[0].loadVertexData();
    GLuint SpadeFishVAO = setBuffers(spadeFishVertexData);
    spadeFishes[0].setAttribPointer();

    vector<GLfloat> troutVertexData = trouts[0].loadVertexData();
    GLuint TroutVAO = setBuffers(troutVertexData);
    trouts[0].setAttribPointer();

    vector<GLfloat> angelFishVertexData = angelFishes[0].loadVertexData();
    GLuint AngelFishVAO = setBuffers(angelFishVertexData);
    angelFishes[0].setAttribPointer();

    // Vertex Sizes
    int bassSize = bassVertexData.size();
    int sharkSize = sharkVertexData.size();
    int whaleSize = whaleVertexData.size();
    int submarineSize = submarineVertexData.size();
    int blueBettaSize = blueBettaVertexData.size();
    int spadeFishSize = spadeFishVertexData.size();
    int troutSize = troutVertexData.size();
    int angelFishSize = angelFishVertexData.size();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Create Texture Buffer Objects // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "> Loading Texture Data...\n";

    MyTextureMap bassTexMap = basses[0].loadTextures();
    MyTextureMap sharkTexMap = shark.loadTextures();
    MyTextureMap whaleTexMap = whale.loadTextures();
    MyTextureMap submarineTexMap = submarine.loadTextures();
    MyTextureMap blueBettaTexMap = blueBettas[0].loadTextures();
    MyTextureMap spadeFishTexMap = spadeFishes[0].loadTextures();
    MyTextureMap troutTexMap = trouts[0].loadTextures();
    MyTextureMap angelFishTexMap = angelFishes[0].loadTextures();


    // Setup Shaders // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "> Loading Shader Data...\n";

    MyShader SMLitTexturedNormap = MyShader("Shaders/lit_textured_normap.vert", "Shaders/lit_textured_normap.frag");
    MyShader SMLitTextured = MyShader("Shaders/lit_textured.vert", "Shaders/lit_textured.frag");

    // Setup Camera (Temp Setup)
    cout << "> Loading Camera Data...\n";

    MyCamera camera = MyCamera(vec3(5.0f, 0.0f, 10.0f), screenWidth, screenHeight);
    camera.center.z = 10.0f;

    // Setup Lighting
    cout << "> Loading Lighting Data...\n";

    vec3 light_color = vec3(1.0f, 1.0f, 1.0f);
    DirectionalLight directional_light = DirectionalLight(vec3(0.0f, 50.0f, 0.0f), light_color, 1.0f);
    PointLight point_light = PointLight(vec3(0.0f, 30.0f, -100.0f), light_color, 1.0f);

    // Debugging Controls
    cRotation = &spadeFishes[0].rotation;


    // For Loops Counters
    int numBass = basses.size();
    int numBetta = blueBettas.size();
    int numSpadeFish = spadeFishes.size();
    int numTrout = trouts.size();
    int numAngelFish = angelFishes.size();

    // TODO: Scale ALL Objects Accordingly

    cout << "> Drawing...\n";
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        // Whale
        whale.draw(SMLitTexturedNormap, whaleSize, WhaleVAO, whaleTexMap, camera, directional_light, point_light);
        whale.position.z = (whale.position.z > 600.0f) ? (-600.0f) : (whale.position.z + 0.40f);

        // Shark
        shark.draw(SMLitTexturedNormap, sharkSize, SharkVAO, sharkTexMap, camera, directional_light, point_light);
        shark.position.z = (shark.position.z > 80.0f) ? (-80.0f) : (shark.position.z + 0.15f);

        // Spade Fish
        for (int i = 0; i < numSpadeFish; i++) {
            spadeFishes[i].draw(SMLitTextured, spadeFishSize, SpadeFishVAO, spadeFishTexMap, camera, directional_light, point_light);
            spadeFishes[i].position.z = fmod(spadeFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
        }

        // Bass
        for (int i = 0; i < numBass; i++) {
            basses[i].draw(SMLitTextured, bassSize, BassVAO, bassTexMap, camera, directional_light, point_light);
            basses[i].position.z = fmod(basses[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
        }

        // Trout
        for (int i = 0; i < numBass; i++) {
            trouts[i].draw(SMLitTexturedNormap, troutSize, TroutVAO, troutTexMap, camera, directional_light, point_light);
            trouts[i].position.z = fmod(trouts[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
        }

        // Blue Betta
        for (int i = 0; i < numBetta; i++) {
            blueBettas[i].draw(SMLitTexturedNormap, blueBettaSize, BlueBettaVAO, blueBettaTexMap, camera, directional_light, point_light);
            blueBettas[i].position.z = fmod(blueBettas[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
        }

        // Angel Fish
        for (int i = 0; i < numAngelFish; i++) {
            angelFishes[i].draw(SMLitTextured, angelFishSize, AngelFishVAO, angelFishTexMap, camera, directional_light, point_light);
            angelFishes[i].position.z = fmod(angelFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
        }

        // Submarine
        submarine.draw(SMLitTexturedNormap, submarineSize, SubmarineVAO, submarineTexMap, camera, directional_light, point_light);
        submarine.position.z = (submarine.position.z > 80.0f) ? (-80.0f) : (submarine.position.z + 0.5f);

        // Light Intensity
        switch (intensity_level) {
            case 1: point_light.intensity = 0.2f; break;
            case 2: point_light.intensity = 1.0f; break;
            case 3: point_light.intensity = 3.0f; break;
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}