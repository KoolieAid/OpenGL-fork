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

#define PI 3.141592653589

// Global Parameters & Settings // // // // // // // // // // // // // // // // // // // // // // // // 

// For Controlling the Camera View
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

// Player Controls
AngelFish* playerControl;
vec3 playerFront = vec3(0.0f, 0.0f, 1.0f);
vec3 direction = vec3(0.0f);
float controlYaw = -90.0f;

// Camera Control Pointers
OrthoCamera* orthoControl;
PerspectiveCamera* POV3Control;
PerspectiveCamera* POV1Control;
float moveSpeed = 0.1f;
float turnSpeed = 2.5f;

// Setting
int intensity_level = 2;  

// For Debugging
void print(vec3 vector)
{
    cout << vector.x << "-" << vector.z << "-" << vector.z << "\n";
}

// Determines the Front of the Model when Moving
void turn()
{ 
    vec3 playerDirection;

    playerDirection.x = cos(PI * 2 * controlYaw / 360.0f);
    playerDirection.y = 0.0f;
    playerDirection.z = sin(PI * 2 * controlYaw / 360.0f);

    playerFront = glm::normalize(playerDirection);
}

// Key Callbacks [Controls?/Debugging] // // // // // // // // // // // // // // // // // // // // // // // // 
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) 
{
    // Camera Mode Controls
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        if (isPOV3) {
            isPOV3 = isOrtho = false;
            isPOV1 = true;
            std::cout << "Camera is in 1st POV - perspective view.\n";
        }
        else if (isPOV3 == false) {
            isPOV1 = isOrtho = false;
            isPOV3 = true;
            std::cout << "Camera is in 3rd POV - perspective view.\n";
        }
    } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        isPOV1 = isPOV3 = false;
        isOrtho = true;
        std::cout << "Camera is in orthographic view.\n";
    }

    // Camera Movement
    if (isPOV3) {                                                       // 3rd Perspective
        switch (key) {
            case GLFW_KEY_W: playerControl->position -= playerFront * moveSpeed;
                break;
            case GLFW_KEY_A: playerControl->rotation.y += turnSpeed;
                             controlYaw -= turnSpeed;
                             turn(); 
                break;
            case GLFW_KEY_S: playerControl->position += playerFront * moveSpeed;
                break;
            case GLFW_KEY_D: playerControl->rotation.y -= turnSpeed;  
                             controlYaw += turnSpeed;
                             turn();  
                break;
            case GLFW_KEY_Q: playerControl->position.y += moveSpeed;
                             POV3Control->position.y += moveSpeed;
                break;
            case GLFW_KEY_E: playerControl->position.y -= moveSpeed;
                             POV3Control->position.y -= moveSpeed;
                break;
        }
        POV1Control->center = playerControl->position - playerFront * 2.0f;
        POV1Control->position = playerControl->position;
        POV3Control->position = playerControl->position + glm::normalize(-direction);
    }
    else if (isPOV1) {                                                  // 1st Perspective
        switch (key) {
            case GLFW_KEY_W: playerControl->position -= playerFront * moveSpeed;
                break;
            case GLFW_KEY_A: playerControl->rotation.y += turnSpeed;
                             controlYaw -= turnSpeed;
                             turn();
                break;
            case GLFW_KEY_S: playerControl->position += playerFront * moveSpeed;
                break;
            case GLFW_KEY_D: playerControl->rotation.y -= turnSpeed;
                             controlYaw += turnSpeed;
                             turn();
                break;
            case GLFW_KEY_Q: playerControl->position.y += moveSpeed;
                             POV1Control->center.y += moveSpeed;
                break;
            case GLFW_KEY_E: playerControl->position.y -= moveSpeed;
                             POV1Control->center.y -= moveSpeed;
                break;
        }
        POV1Control->center = playerControl->position - playerFront * 2.0f;
        POV1Control->position = playerControl->position;
        POV3Control->position = playerControl->position + glm::normalize(-direction);
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

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        intensity_level++;
        if (intensity_level == 4) {
            intensity_level = 1;
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

        // Update Direction
        direction.x = cos(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));
        direction.y = sin(glm::radians(cam_pitch));
        direction.z = sin(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));

        POV3Control->center = glm::normalize(direction);
        POV3Control->position = playerControl->position + glm::normalize(-direction);
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

    // Creature Models
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

    vector<Trout> trouts;
    for (int i = 0; i < 12; i++) {
        trouts.push_back(Trout(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5), vec3(troutScale), vec3(0.0f, 0.0f, 0.0f)));
    }

    Shark shark = Shark(vec3(-12.0f, 0.0f, 10.0f), vec3(sharkScale), vec3(0.0, 0.0f, 2.0f));
    Whale whale = Whale(vec3(20.0f, -20.0f, -10.0f), vec3(whaleScale), vec3(0.0f, 0.0f, 0.0f));
    Submarine submarine = Submarine(vec3(10.0f, -10.0f, 10.0f), vec3(submarineScale), vec3(0.0f));

    // Skybox
    SkyBox skybox = SkyBox();

    // Player Model
    AngelFish angelFish = AngelFish(vec3(0.0f, 0.0f, 0.0f), vec3(angelFishScale, angelFishScale, angelFishScale), vec3(0.0f, controlYaw, 0.0f));
    playerControl = &angelFish;


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

    vector<GLfloat> angelFishVertexData = angelFish.loadVertexData();
    GLuint AngelFishVAO = setBuffers(angelFishVertexData);
    angelFish.setAttribPointer();

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
    MyTextureMap angelFishTexMap = angelFish.loadTextures();

    cout << "> Loading SkyBox Texture Data...\n";
    GLuint skyboxTex = skybox.loadTextures();


    // Setup Shaders // // // // // // // // // // // // // // // // // // // // // // // //
    cout << "> Loading Shader Data...\n";

    MyShader SMLitTexturedNormap = MyShader("Shaders/lit_textured_normap.vert", "Shaders/lit_textured_normap.frag");
    MyShader SMLitTextured = MyShader("Shaders/lit_textured.vert", "Shaders/lit_textured.frag");
    MyShader SMSkyBox = MyShader("Shaders/skybox.vert", "Shaders/skybox.frag");

    // Setup Camera (Temp Setup)
    cout << "> Loading Camera Data...\n";
    
    // Initial Player Direction
    turn();

    // Orthographic Camera
    OrthoCamera orthoCam = OrthoCamera(vec3(1.0f, 4.0f, 1.0f), screenWidth, screenHeight);
    orthoCam.center = vec3(0.5f, 0.0f, 0.5f);
    orthoControl = &orthoCam;

    // 3rd Person Perspective Camera
    PerspectiveCamera POV3Cam = PerspectiveCamera(playerControl->position - playerFront, screenWidth, screenHeight);
    POV3Cam.zfar = 30.0f;
    POV3Cam.center = playerControl->position;
    POV3Control = &POV3Cam;

    // 1st Person Perspective Camera
    PerspectiveCamera POV1Cam = PerspectiveCamera(playerControl->position, screenWidth, screenHeight);
    POV1Cam.zfar = 80.0f;
    POV1Cam.center = playerControl->position - playerFront * 2.0f;
    POV1Control = &POV1Cam;

    // Setup Lighting
    cout << "> Loading Lighting Data...\n";

    vec3 light_color = vec3(1.0f, 1.0f, 1.0f);
    DirectionalLight directional_light = DirectionalLight(vec3(0.0f, 50.0f, 0.0f), light_color, 1.0f);
    PointLight point_light = PointLight(vec3(angelFish.position.x, angelFish.position.y + 1.0f, angelFish.position.z), light_color, 1.0f);

    // For Loops Counters
    int numBass = basses.size();
    int numBetta = blueBettas.size();
    int numSpadeFish = spadeFishes.size();
    int numTrout = trouts.size();


    cout << "> Drawing...\n";
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // 3rd POV - perspective camera -------------------------------------------------------------------------------------------------------------------------------------------------------------
        if (isPOV3) {

            // Disables blending
            glDisable(GL_BLEND);

            // Skybox
            skybox.draw(SMSkyBox, skyboxTex, POV3Cam.persProject(), POV3Cam.persViewPOV3());

            // Whale
            whale.draw(SMLitTexturedNormap, whaleSize, WhaleVAO, whaleTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
            whale.position.z = (whale.position.z > 100.0f) ? (-100.0f) : (whale.position.z + 0.40f);

            // Shark
            shark.draw(SMLitTexturedNormap, sharkSize, SharkVAO, sharkTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
            shark.position.z = (shark.position.z > 50.0f) ? (-50.0f) : (shark.position.z + 0.15f);

            // Submarine
            submarine.draw(SMLitTexturedNormap, submarineSize, SubmarineVAO, submarineTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
            submarine.position.z = (submarine.position.z > 50.0f) ? (-50.0f) : (submarine.position.z + 0.5f);

            // Spade Fish
            for (int i = 0; i < numSpadeFish; i++) {
                spadeFishes[i].draw(SMLitTextured, spadeFishSize, SpadeFishVAO, spadeFishTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
                spadeFishes[i].position.z = fmod(spadeFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Bass
            for (int i = 0; i < numBass; i++) {
                basses[i].draw(SMLitTextured, bassSize, BassVAO, bassTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
                basses[i].position.z = fmod(basses[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Trout
            for (int i = 0; i < numBass; i++) {
                trouts[i].draw(SMLitTexturedNormap, troutSize, TroutVAO, troutTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
                trouts[i].position.z = fmod(trouts[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Blue Betta
            for (int i = 0; i < numBetta; i++) {
                blueBettas[i].draw(SMLitTexturedNormap, blueBettaSize, BlueBettaVAO, blueBettaTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
                blueBettas[i].position.z = fmod(blueBettas[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // AngelFish (Player)
            angelFish.draw(SMLitTextured, angelFishSize, AngelFishVAO, angelFishTexMap, POV3Cam.persProject(), POV3Cam.persViewPOV3(), directional_light, point_light);
        }

        // Ortho Camera -------------------------------------------------------------------------------------------------------------------------------------------------------------
        else if (isOrtho) {

            // Disables blending
            glDisable(GL_BLEND);

            // Skybox
            skybox.draw(SMSkyBox, skyboxTex, orthoCam.orthoProjectSkybox(), orthoCam.orthoView());

            // AngelFish (Player)
            angelFish.draw(SMLitTextured, angelFishSize, AngelFishVAO, angelFishTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);

            // Whale
            whale.draw(SMLitTexturedNormap, whaleSize, WhaleVAO, whaleTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
            whale.position.z = (whale.position.z > 100.0f) ? (-100.0f) : (whale.position.z + 0.40f);

            // Shark
            shark.draw(SMLitTexturedNormap, sharkSize, SharkVAO, sharkTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
            shark.position.z = (shark.position.z > 50.0f) ? (-50.0f) : (shark.position.z + 0.15f);

            // Submarine
            submarine.draw(SMLitTexturedNormap, submarineSize, SubmarineVAO, submarineTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
            submarine.position.z = (submarine.position.z > 50.0f) ? (-50.0f) : (submarine.position.z + 0.5f);

            // Spade Fish
            for (int i = 0; i < numSpadeFish; i++) {
                spadeFishes[i].draw(SMLitTextured, spadeFishSize, SpadeFishVAO, spadeFishTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
                spadeFishes[i].position.z = fmod(spadeFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Bass
            for (int i = 0; i < numBass; i++) {
                basses[i].draw(SMLitTextured, bassSize, BassVAO, bassTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
                basses[i].position.z = fmod(basses[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Trout
            for (int i = 0; i < numBass; i++) {
                trouts[i].draw(SMLitTexturedNormap, troutSize, TroutVAO, troutTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
                trouts[i].position.z = fmod(trouts[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Blue Betta
            for (int i = 0; i < numBetta; i++) {
                blueBettas[i].draw(SMLitTexturedNormap, blueBettaSize, BlueBettaVAO, blueBettaTexMap, orthoCam.orthoProject(), orthoCam.orthoView(), directional_light, point_light);
                blueBettas[i].position.z = fmod(blueBettas[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }
        }

        // 1st POV - perspective camera -------------------------------------------------------------------------------------------------------------------------------------------------------------
        else if (isPOV1) {

            // Apply sonar view through blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_CONSTANT_COLOR);
            glBlendEquation(GL_FUNC_ADD);
            glBlendColor(1.000, 0.012, 0.012, 1.000);

            // Skybox
            skybox.draw(SMSkyBox, skyboxTex, POV1Cam.persProject(), POV1Cam.persViewPOV1());

            // AngelFish (Player)
            // There is no need to draw the Player in POV1

            // Whale
            whale.draw(SMLitTexturedNormap, whaleSize, WhaleVAO, whaleTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
            whale.position.z = (whale.position.z > 100.0f) ? (-100.0f) : (whale.position.z + 0.40f);

            // Shark
            shark.draw(SMLitTexturedNormap, sharkSize, SharkVAO, sharkTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
            shark.position.z = (shark.position.z > 50.0f) ? (-50.0f) : (shark.position.z + 0.15f);

            // Submarine
            submarine.draw(SMLitTexturedNormap, submarineSize, SubmarineVAO, submarineTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
            submarine.position.z = (submarine.position.z > 50.0f) ? (-50.0f) : (submarine.position.z + 0.5f);

            // Spade Fish
            for (int i = 0; i < numSpadeFish; i++) {
                spadeFishes[i].draw(SMLitTextured, spadeFishSize, SpadeFishVAO, spadeFishTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
                spadeFishes[i].position.z = fmod(spadeFishes[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Bass
            for (int i = 0; i < numBass; i++) {
                basses[i].draw(SMLitTextured, bassSize, BassVAO, bassTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
                basses[i].position.z = fmod(basses[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Trout
            for (int i = 0; i < numBass; i++) {
                trouts[i].draw(SMLitTexturedNormap, troutSize, TroutVAO, troutTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
                trouts[i].position.z = fmod(trouts[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }

            // Blue Betta
            for (int i = 0; i < numBetta; i++) {
                blueBettas[i].draw(SMLitTexturedNormap, blueBettaSize, BlueBettaVAO, blueBettaTexMap, POV1Cam.persProject(), POV1Cam.persViewPOV1(), directional_light, point_light);
                blueBettas[i].position.z = fmod(blueBettas[i].position.z, 20.0f) + ((i + 1) % 10 / 100.0f);
            }
        }
        
        // Light Intensity
        switch (intensity_level) {
            case 1: point_light.intensity = 1.0f; break;
            case 2: point_light.intensity = 3.0f; break;
            case 3: point_light.intensity = 6.0f; break;
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
