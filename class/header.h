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

// Global Variables

// Default Value for Virtual Functions
GLuint def = 69;

// Rotation Matrices
vec3 rx = normalize(glm::vec3(1, 0, 0));
vec3 ry = normalize(glm::vec3(0, 1, 0));
vec3 rz = normalize(glm::vec3(0, 0, 1));

// World Up Matrix
vec3 up = vec3(0.0f, 1.0f, 0.0f);

// // // // // // // // // // // // // // // // // // // // // // // // // 

// Setup VAO & VBO Buffers
GLuint setBuffers(vector<GLfloat> fullVertexData)
{
	// Setup Vertex Array Object
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Setup Vertex Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * fullVertexData.size(), fullVertexData.data(), GL_STATIC_DRAW);

	return VAO;
}

// // // // // // // // // // // // // // // // // // // // // // // // // 

class MyShader
{
public:
	// Shader Program
	GLuint shaderProgram = glCreateProgram();;

	// Constructor
	MyShader(const char* vertex, const char* fragment)
	{
		addVertexShader(vertex);
		addFragmentShader(fragment);
		link();
	}

	// Use Program
	void activate()
	{
		glUseProgram(shaderProgram);
	}

private:
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

// // // // // // // // // // // // // // // // // // // // // // // // // 

// Camera Class
class MyCamera
{
	public:
		// Position
		vec3 position = vec3(5.0f, 0.0f, 0.0f);
		vec3 center = vec3(0.0f, 0.0f, 0.0f);

		// Display Resolution
		float height = 1920.0f;
		float width = 1080.0f;

		// Constructors
		MyCamera() {}

		MyCamera(vec3 nposition, float nheight, float nwidth)
		{
			position = nposition;
			height = nheight;
			width = nwidth;
		}

		// Temp Methods (This class is passed onto the draw function of 3D object classes)
		mat4 project()
		{
			return perspective(radians(60.0f), height / width, 0.01f, 1000.0f);
		}

		mat4 view()
		{
			return lookAt(position, center, up);
		}

		/*
			The game should feature the following Camera�s
			- 3rd Person Perspective Camera on the Player�s ship
			- The view can be controlled by using the mouse, You cannot see as far in this view
			- First Person Perspective Camera, You can see much further in this view, Cannot be controlled by the mouse
			- You can only see objects in a single shade of color in this view (Similar to Sonar)

			Orthographic Top / Birds-eye View Camera overlooking the whole area by default
			- You cannot move the ship in this view
			- You can pan the camera around using WASD

			- You can swap 1st / 3rd Person Views using the number 1 key.
			- You can enter Top / Birds-eye view using the number 2 key.
		*/
};

// // // // // // // // // // // // // // // // // // // // // // // // // 

class MyLight
{
	/*
		The game should feature the following lighting :
		- Point Light that illuminates the front of the player sub.
		- Direction Light coming from the top of the ocean down
		- You can cycle through the intensity of the Point light using the F key (Low, Medium, High)
	*/
};

// // // // // // // // // // // // // // // // // // // // // // // // // 

// 3D Model Object Base Class
class MyObject 
{
	public:
		// Position, Scale, Rotation (thetas)
		vec3 position = vec3(0.0f, 0.0f, 0.0f);
		vec3 scale = vec3(1.0f, 1.0f, 1.0f);
		vec3 rotation = vec3(0.0f, 0.0f, 0.0f);

		// Constructors
		MyObject() {}

		MyObject(vec3 nposition, vec3 nscale, vec3 nrotation)
		{
			position = nposition;
			scale = nscale;
			rotation = nrotation;
		}

		// Virtual Functions

		// Load Vertex Data 
		virtual vector<GLfloat> loadVertexData()
		{
			cout << "Load Vertex Data function is not defined!";
			vector<GLfloat> temp;
			return temp;
		}

		// Load Texture Data
		virtual GLuint loadTextures()
		{
			cout << "Load Texture Data function is not defined!";
			return def;
		}

		// Setup Attrib Pointers 
		virtual void setAttribPointer()
		{
			cout << "Set Attrib Pointers function is not defined!";
		}

		// Draw
		virtual void draw() 
		{
			cout << "Draw function is not defined!";
		}

	protected:
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

// Bass Model Object Class
class Bass : public MyObject
{
	public:
		// Path
		string path = "3D/bass.obj";

		// Constructors
		Bass() : MyObject() {}

		Bass(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

		// Load Vertex Data 
		vector<GLfloat> loadVertexData()
		{
			// Loader variables
			vector<tinyobj::shape_t> shapes;
			vector<tinyobj::material_t> materials;
			string warning, error;
			tinyobj::attrib_t attributes;

			// Load the obj file
			bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());

			vector<GLfloat> fullVertexData;
			for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
				tinyobj::index_t vData = shapes[0].mesh.indices[i];
				int vertexIndex = vData.vertex_index * 3;
				int normIndex = vData.normal_index * 3;
				int texIndex = vData.texcoord_index * 2;

				// Get X Y Z
				fullVertexData.push_back(attributes.vertices[vertexIndex]);
				fullVertexData.push_back(attributes.vertices[vertexIndex + 1]);
				fullVertexData.push_back(attributes.vertices[vertexIndex + 2]);

				// Get A B C
				fullVertexData.push_back(attributes.normals[normIndex]);
				fullVertexData.push_back(attributes.normals[normIndex + 1]);
				fullVertexData.push_back(attributes.normals[normIndex + 2]);

				// Get U V
				fullVertexData.push_back(attributes.texcoords[texIndex]);
				fullVertexData.push_back(attributes.texcoords[texIndex + 1]);
			}

			return fullVertexData;
		}

		// Load Textures :: GL_TEXTURE_0
		GLuint loadTextures()
		{
			GLuint texture;
			int img_width, img_height, color_channels;
			unsigned char* tex_bytes = stbi_load("Textures/bass_tex.png", &img_width, &img_height, &color_channels, 0);
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(tex_bytes);

			return texture;
		}

		// Setup Attrib Pointers 
		void setAttribPointer()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(0);

			GLintptr abcptr = 3 * sizeof(GL_FLOAT);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)abcptr);
			glEnableVertexAttribArray(1);

			GLintptr uvptr = 6 * sizeof(GL_FLOAT);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)uvptr);
			glEnableVertexAttribArray(2);
		}

		// Draw
		void draw(MyShader shader, float vsize, GLuint VAO, GLuint texture, MyCamera camera)
		{
			// Shader Program
			shader.activate();

			// Create Transformation Matrix
			mat4 transformation = transform();
			unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

			// Projection & View Matrices
			unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
			glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(camera.project()));

			unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.view()));

			// Texture
			GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(tex0Address, 0);

			// Bind
			glBindVertexArray(VAO);

			// Draw
			glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
		}
};

// Shark Model Object Class
class Shark : public MyObject
{
public:
	// Path
	string path = "3D/shark.obj";

	// Constructors
	Shark() : MyObject() {}

	Shark(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

	// Load Vertex Data 
	vector<GLfloat> loadVertexData()
	{
		// Loader variables
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string warning, error;
		tinyobj::attrib_t attributes;

		// Load the obj file
		bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());

		vector<GLfloat> fullVertexData;
		for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
			tinyobj::index_t vData = shapes[0].mesh.indices[i];
			int vertexIndex = vData.vertex_index * 3;
			int normIndex = vData.normal_index * 3;
			int texIndex = vData.texcoord_index * 2;

			// Get X Y Z
			fullVertexData.push_back(attributes.vertices[vertexIndex]);
			fullVertexData.push_back(attributes.vertices[vertexIndex + 1]);
			fullVertexData.push_back(attributes.vertices[vertexIndex + 2]);

			// Get A B C
			fullVertexData.push_back(attributes.normals[normIndex]);
			fullVertexData.push_back(attributes.normals[normIndex + 1]);
			fullVertexData.push_back(attributes.normals[normIndex + 2]);

			// Get U V
			fullVertexData.push_back(attributes.texcoords[texIndex]);
			fullVertexData.push_back(attributes.texcoords[texIndex + 1]);
		}

		return fullVertexData;
	}

	// Load Textures :: GL_TEXTURE_1
	GLuint loadTextures()
	{
		GLuint texture;
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/shark_tex.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		return texture;
	}

	// Setup Attrib Pointers 
	void setAttribPointer()
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);

		GLintptr abcptr = 3 * sizeof(GL_FLOAT);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)abcptr);
		glEnableVertexAttribArray(1);

		GLintptr uvptr = 6 * sizeof(GL_FLOAT);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)uvptr);
		glEnableVertexAttribArray(2);
	}

	// Draw
	void draw(MyShader shader, float vsize, GLuint VAO, GLuint texture, MyCamera camera)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(camera.project()));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.view()));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(tex0Address, 0);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// // // // // // // // // // // // // // // // // // // // // // // // // 

class Player : public MyObject
{
	/*
		The Player ship can be controlled using WASDQE
		- W / S � Forward / Back
		- A / D � Turn Left / Right
		- Q / E � Ascend / Descend

		- Sub can only be controlled when in 1st / 3rd Person view
		- Sub cannot go above 0 in the Y axis
		-  Print out the current depth the sub is in the console window using cout
	*/
};


class SkyBox : public MyObject
{
public:
	// Path
	string path = "3D/skybox.obj";

	// Constructors (Inherits MyObject)
	SkyBox() {}
	

	// Load Textures
	GLuint loadTextures()
	{

	}
};

// // // // // // // // // // // // // // // // // // // // // // // // // 

/*	Others Materials
* 
	https://antongerdelan.net/colour/ :: float vector color picker



*/




