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

// For controlling the camera view
bool isOrtho = false;
bool isPOV3 = true;
bool isPOV1 = false;

// Mouse
float lastX = 960.0f, lastY = 540.0f;
bool firstMouse = true;
float cam_yaw = 90.0f;
float cam_pitch = 0.0f;

// Screen size
float screenWidth = 1920.0f;
float screenHeight = 1080.0f;

// Camera Control Pointers
OrthoCamera* orthoControl;
PerspectiveCamera* POV3Control;
PerspectiveCamera* POV1Control;
float moveSpeed = 0.25f;

// Key Callbacks [Controls?/Debugging] // // // // // // // // // // // // // // // // // // // // // // // // 
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) 
{
    // Camera Mode Controls
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        if (isPOV3) {
            isPOV3 = false;
            isOrtho = false;
            isPOV1 = true;
            std::cout << "Camera is in 3rd POV - perspective view.\n";
        }
        else if (isPOV3 == false) {
            isPOV3 = true;
            isOrtho = false;
            isPOV1 = false;
            std::cout << "Camera is in 1st POV - perspective view.\n";
        }
    } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        isOrtho = true;
        isPOV3 = false;
        isPOV1 = false;
        std::cout << "Camera is in orthographic view.\n";
    }

    // Camera Movement
    if (isPOV3) {                                                       // 3rd Perspective
        switch (key) {
            case GLFW_KEY_W: POV3Control->position.z += moveSpeed;
                break;
            case GLFW_KEY_A: POV3Control->position.x -= moveSpeed;
                break;
            case GLFW_KEY_S: POV3Control->position.z -= moveSpeed;
                break;
            case GLFW_KEY_D: POV3Control->position.x += moveSpeed;
                break;
        }
    }
    else if (isPOV1) {                                                  // 1st Perspective
        switch (key) {
            case GLFW_KEY_W: POV1Control->position.z += moveSpeed;
                break;
            case GLFW_KEY_A: POV1Control->position.x -= moveSpeed;
                break;
            case GLFW_KEY_S: POV1Control->position.z -= moveSpeed;
                break;
            case GLFW_KEY_D: POV1Control->position.x += moveSpeed;
                break;
        }
    }
    else {                                                              // Orthographic
        switch (key) {
            case GLFW_KEY_W: orthoControl->position.z += moveSpeed;
                             orthoControl->center.z += moveSpeed;
                break;
            case GLFW_KEY_A: orthoControl->position.x -= moveSpeed;
                             orthoControl->center.x -= moveSpeed;
                break;
            case GLFW_KEY_S: orthoControl->position.z -= moveSpeed;
                             orthoControl->center.z -= moveSpeed;
                break;
            case GLFW_KEY_D: orthoControl->position.x += moveSpeed;
                             orthoControl->center.x += moveSpeed;
                break;
        }
    }
}

// Mouse controls when in 3rd POV - perspective camera
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Perspective Controls
    if (isPOV3) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        cam_yaw += xoffset;
        cam_pitch += yoffset;

        if (cam_pitch > 89.0f)
            cam_pitch = 89.0f;
        if (cam_pitch < -89.0f)
            cam_pitch = -89.0f;

        vec3 direction;
        direction.x = cos(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));
        direction.y = sin(glm::radians(cam_pitch));
        direction.z = sin(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));

        POV3Control->center = glm::normalize(direction);
    }
    else if (isPOV1) {
        //POV1Control->center = glm::normalize(direction);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetCursorPos(window, lastX, lastY);


    // Load Object Models // // // // // // // // // // // // // // // // // // // // // // // //

    // Object Scales
    float angelFishScale = 1.0f / 4.0f;
    float sharkScale = 1.0f / 192.0;
    float blueBettaScale = 1.0f / 600.0f;
    float troutScale = 1.0f / 74.0f;
    float bassScale = 1.0f / 8.0f;
    float spadeFishScale = 1.0f / 512.0f;
    float whaleScale = 1.0f;
    float submarineScale = 1.0 / 96.0f;

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

    Shark shark = Shark(vec3(-12.0f, 0.0f, 10.0f), vec3(sharkScale), vec3(0.0, 0.0f, 2.0f));
    Whale whale = Whale(vec3(20.0f, -20.0f, -10.0f), vec3(whaleScale), vec3(0.0f, 0.0f, 0.0f));
    Submarine submarine = Submarine(vec3(10.0f, -10.0f, 10.0f), vec3(submarineScale), vec3(0.0f));

   
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

    MyShader SMBass = MyShader("Shaders/bass.vert", "Shaders/bass.frag");
    MyShader SMShark = MyShader("Shaders/shark.vert", "Shaders/shark.frag");
    MyShader SMWhale = MyShader("Shaders/whale.vert", "Shaders/whale.frag");
    MyShader SMSubmarine = MyShader("Shaders/submarine.vert", "Shaders/submarine.frag");
    MyShader SMBlueBetta = MyShader("Shaders/blue_betta.vert", "Shaders/blue_betta.frag");
    MyShader SMSailFish = MyShader("Shaders/spade_fish.vert", "Shaders/spade_fish.frag");
    MyShader SMTrout = MyShader("Shaders/trout.vert", "Shaders/trout.frag");
    MyShader SMAngelFish = MyShader("Shaders/angel_fish.vert", "Shaders/angel_fish.frag");


    // Setup Camera (Temp Setup)
    cout << "> Loading Camera Data...\n";

    // Orthographic Camera
    OrthoCamera orthoCam = OrthoCamera(vec3(1.0f, 4.0f, 1.0f), screenWidth, screenHeight);
    orthoCam.center = vec3(0.5f, 0.0f, 0.5f);
    orthoControl = &orthoCam;

    // 3rd Person Perspective Camera
    PerspectiveCamera POV3Cam = PerspectiveCamera(vec3(0.0f, 0.0f, 0.5f), screenWidth, screenHeight);
    POV3Cam.center = vec3(0.0f);
    POV3Control = &POV3Cam;

    // 1st Person Perspective Camera (IF NEEDED)
    // TODO:


    // Setup Lighting (Temp Setup)
    cout << "> Loading Lighting Data...\n";

    // For Loops Counters
    int numBass = basses.size();
    int numBetta = blueBettas.size();
    int numSpadeFish = spadeFishes.size();
    int numTrout = trouts.size();
    int numAngelFish = angelFishes.size();


    cout << "> Drawing...\n";
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // 3rd POV - perspective camera -------------------------------------------------------------------------------------------------------------------------------------------------------------
        if (isPOV3) {
            // Whale
            whale.draw(SMWhale, whaleSize, WhaleVAO, whaleTexMap, POV3Cam.persProject(), POV3Cam.persView());
            whale.position.z = (whale.position.z > 100.0f) ? (-100.0f) : (whale.position.z + 0.40f);

            // Shark
            shark.draw(SMShark, sharkSize, SharkVAO, sharkTexMap, POV3Cam.persProject(), POV3Cam.persView());
            shark.position.z = (shark.position.z > 50.0f) ? (-50.0f) : (shark.position.z + 0.15f);

            // Submarine
            submarine.draw(SMSubmarine, submarineSize, SubmarineVAO, submarineTexMap, POV3Cam.persProject(), POV3Cam.persView());
            submarine.position.z = (submarine.position.z > 50.0f) ? (-50.0f) : (submarine.position.z + 0.5f);

            // Spade Fish
            for (int i = 0; i < numSpadeFish; i++) {
                spadeFishes[i].draw(SMSailFish, spadeFishSize, SpadeFishVAO, spadeFishTexMap, POV3Cam.persProject(), POV3Cam.persView());
                spadeFishes[i].position.z = fmod(spadeFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Bass
            for (int i = 0; i < numBass; i++) {
                basses[i].draw(SMBass, bassSize, BassVAO, bassTexMap, POV3Cam.persProject(), POV3Cam.persView());
                basses[i].position.z = fmod(basses[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Trout
            for (int i = 0; i < numBass; i++) {
                trouts[i].draw(SMTrout, troutSize, TroutVAO, troutTexMap, POV3Cam.persProject(), POV3Cam.persView());
                trouts[i].position.z = fmod(trouts[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Blue Betta
            for (int i = 0; i < numBetta; i++) {
                blueBettas[i].draw(SMBlueBetta, blueBettaSize, BlueBettaVAO, blueBettaTexMap, POV3Cam.persProject(), POV3Cam.persView());
                blueBettas[i].position.z = fmod(blueBettas[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Angel Fish
            for (int i = 0; i < numAngelFish; i++) {
                angelFishes[i].draw(SMAngelFish, angelFishSize, AngelFishVAO, angelFishTexMap, POV3Cam.persProject(), POV3Cam.persView());
                angelFishes[i].position.z = fmod(angelFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }
        }

        // Ortho Camera -------------------------------------------------------------------------------------------------------------------------------------------------------------
        else if (isOrtho) {
            // Whale
            whale.draw(SMWhale, whaleSize, WhaleVAO, whaleTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
            whale.position.z = (whale.position.z > 100.0f) ? (-100.0f) : (whale.position.z + 0.40f);

            // Shark
            shark.draw(SMShark, sharkSize, SharkVAO, sharkTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
            shark.position.z = (shark.position.z > 50.0f) ? (-50.0f) : (shark.position.z + 0.15f);

            // Submarine
            submarine.draw(SMSubmarine, submarineSize, SubmarineVAO, submarineTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
            submarine.position.z = (submarine.position.z > 50.0f) ? (-50.0f) : (submarine.position.z + 0.5f);

            // Spade Fish
            for (int i = 0; i < numSpadeFish; i++) {
                spadeFishes[i].draw(SMSailFish, spadeFishSize, SpadeFishVAO, spadeFishTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
                spadeFishes[i].position.z = fmod(spadeFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Bass
            for (int i = 0; i < numBass; i++) {
                basses[i].draw(SMBass, bassSize, BassVAO, bassTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
                basses[i].position.z = fmod(basses[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Trout
            for (int i = 0; i < numBass; i++) {
                trouts[i].draw(SMTrout, troutSize, TroutVAO, troutTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
                trouts[i].position.z = fmod(trouts[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Blue Betta
            for (int i = 0; i < numBetta; i++) {
                blueBettas[i].draw(SMBlueBetta, blueBettaSize, BlueBettaVAO, blueBettaTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
                blueBettas[i].position.z = fmod(blueBettas[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Angel Fish
            for (int i = 0; i < numAngelFish; i++) {
                angelFishes[i].draw(SMAngelFish, angelFishSize, AngelFishVAO, angelFishTexMap, orthoCam.orthoProject(), orthoCam.orthoView());
                angelFishes[i].position.z = fmod(angelFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}