#include <string>
#include <iostream>
using namespace std;

#include <glad/glad.h>          
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// Global Variables

// Rotation Matrices
vec3 rx = normalize(glm::vec3(1, 0, 0));
vec3 ry = normalize(glm::vec3(0, 1, 0));
vec3 rz = normalize(glm::vec3(0, 0, 1));

// 3D Model Object Base Class
class MyObject 
{
	public:
		// Position
		vec3 position = vec3(0.0f, 0.0f, 0.0f);

		// Scale
		vec3 scale = vec3(1.0f, 1.0f, 1.0f);

		// Rotation (theta)
		vec3 rotation = vec3(0.0f, 0.0f, 0.0f);

		// Vertex/Normal/Texture Data
		vector<GLfloat> fullVertexData;

		// Constructor
		MyObject(vec3 nposition, float nscale, vec3 nrotation)
		{
			position = nposition;
			scale = vec3(nscale, nscale, nscale);
			rotation = nrotation;
		}

		// Draw
		void draw() 
		{
			cout << "Draw function is not defined!";
		}

		// Create Transformation Matrix
		mat4 transform() 
		{
			mat4 transformation = mat4(1.0f);
			transformation = translate(transformation, position);
			transformation = glm::scale(transformation, scale);
			transformation = rotate(transformation, radians(rotation.x), rx);
			transformation = rotate(transformation, radians(rotation.y), ry);
			transformation = rotate(transformation, radians(rotation.z), rz);

			return transformation;
		}
};

// Bunny Model Object Class (Sample)
class Bunny : public MyObject
{
	public:
		// Path
		string path = "3D/bunny.obj";

		// Constructor (Inherits MyObject)
		Bunny(vec3 nposition, float nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) 
		{
			// Initialize other loader variables
			vector<tinyobj::shape_t> shapes;
			vector<tinyobj::material_t> materials;
			string warning, error;
			tinyobj::attrib_t attributes;

			// Load the obj file
			bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());

			for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
				tinyobj::index_t vData = shapes[0].mesh.indices[i];
				int vertexIndex = vData.vertex_index * 3;

				// Get X Y Z
				fullVertexData.push_back(attributes.vertices[vertexIndex]);
				fullVertexData.push_back(attributes.vertices[vertexIndex + 1]);
				fullVertexData.push_back(attributes.vertices[vertexIndex + 2]);
			}
		}

		// Draw
		void draw(GLuint shaderProgram)
		{
			// Create Transformation Matrix
			mat4 transformation = transform();
			unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

			// Draw
			glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 3);
		}
};

class Player : public MyObject
{

};

// Skybox Model Object Class
// Reference: https://www.youtube.com/watch?v=8sVvxeKI9Pk
class SkyBox
{

};

// // // // // // // // // // // // // // // // // // // // // // // // // 

class ShaderManager 
{
	public:
		// Shader Program
		GLuint shaderProgram = glCreateProgram();;

		// Constructor
		ShaderManager() {}

		// Add Vertex Shader to the Shader Program
		void addVertexShader(const char* directory)  
		{
			// Load
			std::fstream vertSrc(directory);
			std::stringstream vertBuff;
			vertBuff << vertSrc.rdbuf();
			std::string vertString = vertBuff.str();
			const char* v = vertString.c_str();

			// Create & Compile Vertex Shader
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &v, NULL);
			glCompileShader(vertexShader);

			// Attach Shader
			glAttachShader(shaderProgram, vertexShader);		
		}

		// Add Fragment Shader to the Shader Program
		void addFragmentShader(const char* directory) 
		{
			// Load
			std::fstream fragSrc(directory);
			std::stringstream fragBuff;
			fragBuff << fragSrc.rdbuf();
			std::string fragString = fragBuff.str();
			const char* f = fragString.c_str();

			// Create & Compile Fragment Shader
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &f, NULL);
			glCompileShader(fragmentShader);

			// Attach Shader
			glAttachShader(shaderProgram, fragmentShader);
		}

		// Link Shader Program
		void link() 
		{
			glLinkProgram(shaderProgram);
		}
};

class LightManager
{

};

class CameraManager 
{

};
