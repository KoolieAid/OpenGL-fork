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

// Custom Class (contains all texture IDs for an Object)
class MyTextureMap {
public:
	GLuint baseTex;		// Base Texture						tex0 : GL_TEXTURE0
	GLuint normTex;		// Normal Texture					tex1 : GL_TEXTURE1
	GLuint rougTex;		// Roughness Texture				tex2 : GL_TEXTURE2
	GLuint ambiTex;		// Ambient Occlusion Texture		tex3 : GL_TEXTURE3
	GLuint metaTex;		// Metallic Texture					tex4 : GL_TEXTURE4
};

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
	float width = 1920.0f;
	float height = 1080.0f;

	// Constructors
	MyCamera() {}

	MyCamera(vec3 nposition, float nwidth, float nheight)
	{
		position = nposition;
		height = nheight;
		width = nwidth;
	}

	// Temp Methods (This class is passed onto the draw function of 3D object classes)
	mat4 project()
	{
		return perspective(radians(90.0f), width / height, 0.01f, 1000.0f);
	}

	mat4 view()
	{
		return lookAt(position, center, up);
	}
};

// Ortho Camera class that inherits from the camera base class
class OrthoCamera : public MyCamera 
{
	public:
		// Constructor
		OrthoCamera(vec3 nposition, float nwidth, float nheight) : MyCamera(nposition, nwidth, nheight) {}

		mat4 orthoProject() {
			return ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
		}

		mat4 orthoProjectSkybox() {
			return ortho(-0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f);
		}

		mat4 orthoView()
		{
			return lookAt(position, center, up);
		}
};

// Perspective Camera class that inherits from the camera base class
class PerspectiveCamera : public MyCamera 
{
	public:
		float zfar = 50.0f;

		// Constructor
		PerspectiveCamera(vec3 nposition, float nwidth, float nheight) : MyCamera(nposition, nwidth, nheight) {}

		mat4 persProject()
		{
			return perspective(radians(60.0f), width / height, 0.01f, zfar);
		}

		mat4 persViewPOV3()
		{
			return lookAt(position, center + position, up);
		}

		mat4 persViewPOV1()
		{
			return lookAt(position, center, up);
		}
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

public:
	vec3 light_pos = vec3(0.0f, 0.0f, 0.0f);
	vec3 light_color = vec3(1.0f, 1.0f, 1.0f);

	float ambient_str = 0.2f;
	float spec_str = 5.0f;
	float spec_phong = 16.0f;
	float intensity = 1.0f;

	// Constructor
	MyLight(vec3 nposition, vec3 ncolor, float nintensity) {
		light_pos = nposition;
		light_color = ncolor;
		intensity = nintensity;
	}
};

class DirectionalLight : public MyLight
{
public:
	// Constructor
	DirectionalLight(vec3 nposition, vec3 ncolor, float nintensity) : MyLight(nposition, ncolor, nintensity) {}

};

class PointLight : public MyLight
{
public:
	PointLight(vec3 nposition, vec3 ncolor, float nintensity) : MyLight(nposition, ncolor, nintensity) {}

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
	GLuint loadTextures()
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

	// Load Textures
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/Bass/BassTexture.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		return map;
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

		GLintptr tangentPtr = 8 * sizeof(float);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)tangentPtr);
		glEnableVertexAttribArray(3);

		GLintptr bitangentPtr = 11 * sizeof(float);
		glVertexAttribPointer(4, 3,	GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)bitangentPtr);
		glEnableVertexAttribArray(4);
	}

	// Draw
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

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

	// Load Textures
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/Shark/DefaultMaterial_Base_Color.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		// Normal Texture
		unsigned char* norm_bytes = stbi_load("Textures/Shark/DefaultMaterial_Normal_OpenGL.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.normTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, norm_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(norm_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		GLuint tex1Address = glGetUniformLocation(shader.shaderProgram, "tex1");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);
		glUniform1i(tex1Address, 1);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// Whale Model Object Class
class Whale : public MyObject
{
public:
	// Path
	string path = "3D/blue_whale.obj";

	// Constructors
	Whale() : MyObject() {}

	Whale(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

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

	// Load Textures 
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/BlueWhale/material_Base_Color.jpg", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		// Normal Texture
		unsigned char* norm_bytes = stbi_load("Textures/BlueWhale/material_Normal_OpenGL1.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.normTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, norm_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(norm_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		GLuint tex1Address = glGetUniformLocation(shader.shaderProgram, "tex1");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);
		glUniform1i(tex1Address, 1);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntspecStrLoc = glGetUniformLocation(shader.shaderProgram, "pntspecStr");
		glUniform1f(pntspecStrLoc, point.spec_str);

		GLuint pntspecPhongLoc = glGetUniformLocation(shader.shaderProgram, "pntspecPhong");
		glUniform1f(pntspecPhongLoc, point.spec_phong);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// SeaHorse Model Object Class (Player)
class SeaHorse : public MyObject
{
public:
	// Path
	string path = "3D/sea_horse.obj";

	// Constructors
	SeaHorse() : MyObject() {}

	SeaHorse(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

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

	// Load Textures 
	MyTextureMap loadTextures()
	{ 
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/SeaHorse/alb.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		// Normal Texture
		unsigned char* norm_bytes = stbi_load("Textures/SeaHorse/nm.png", &img_width, &img_height, &color_channels, 4);
		glGenTextures(1, &map.normTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, norm_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(norm_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		GLuint tex1Address = glGetUniformLocation(shader.shaderProgram, "tex1");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);
		glUniform1i(tex1Address, 1);


		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntspecStrLoc = glGetUniformLocation(shader.shaderProgram, "pntspecStr");
		glUniform1f(pntspecStrLoc, point.spec_str);

		GLuint pntspecPhongLoc = glGetUniformLocation(shader.shaderProgram, "pntspecPhong");
		glUniform1f(pntspecPhongLoc, point.spec_phong);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// Blue Betta Model Object Class
class BlueBetta : public MyObject
{
public:
	// Path
	string path = "3D/blue_betta.obj";

	// Constructors
	BlueBetta() : MyObject() {}

	BlueBetta(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

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

	// Load Textures 
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/BlueBetta/texture.jpg", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		// Normal Texture
		unsigned char* norm_bytes = stbi_load("Textures/BlueBetta/texture_norm.jpg", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.normTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, norm_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(norm_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		GLuint tex1Address = glGetUniformLocation(shader.shaderProgram, "tex1");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);
		glUniform1i(tex1Address, 1);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// Spade Fish Model Object Class
class SpadeFish : public MyObject
{
public:
	// Path
	string path = "3D/spadefish.obj";

	// Constructors
	SpadeFish() : MyObject() {}

	SpadeFish(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

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

	// Load Textures 
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/SpadeFish/ESPADON1_Material.001_BaseColor.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// Trout Model Object Class
class Trout : public MyObject
{
public:
	// Path
	string path = "3D/trout.obj";

	// Constructors
	Trout() : MyObject() {}

	Trout(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

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

	// Load Textures 
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/Trout/trout_Trout_BaseColor.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		// Normal Texture
		unsigned char* norm_bytes = stbi_load("Textures/Trout/trout_Trout_Normal.png", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.normTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, norm_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(norm_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		GLuint tex1Address = glGetUniformLocation(shader.shaderProgram, "tex1");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, map.normTex);
		glUniform1i(tex1Address, 1);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntspecStrLoc = glGetUniformLocation(shader.shaderProgram, "pntspecStr");
		glUniform1f(pntspecStrLoc, point.spec_str);

		GLuint pntspecPhongLoc = glGetUniformLocation(shader.shaderProgram, "pntspecPhong");
		glUniform1f(pntspecPhongLoc, point.spec_phong);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

		// Bind
		glBindVertexArray(VAO);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vsize / 8);
	}
};

// Angel Fish Model Object Class
class AngelFish : public MyObject
{
public:
	// Path
	string path = "3D/angel_fish.obj";

	// Constructors
	AngelFish() : MyObject() {}

	AngelFish(vec3 nposition, vec3 nscale, vec3 nrotation) : MyObject(nposition, nscale, nrotation) {}

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

	// Load Textures 
	MyTextureMap loadTextures()
	{
		MyTextureMap map;

		// Base Texture
		int img_width, img_height, color_channels;
		unsigned char* tex_bytes = stbi_load("Textures/AngelFish/texture_140170014617.jpeg", &img_width, &img_height, &color_channels, 0);
		glGenTextures(1, &map.baseTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex_bytes);

		return map;
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
	void draw(MyShader shader, float vsize, GLuint VAO, MyTextureMap map, mat4 projection, mat4 view, DirectionalLight direct, PointLight point)
	{
		// Shader Program
		shader.activate();

		// Create Transformation Matrix
		mat4 transformation = transform();
		unsigned int transformLoc = glGetUniformLocation(shader.shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));

		// Projection & View Matrices
		unsigned int projectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Texture
		GLuint tex0Address = glGetUniformLocation(shader.shaderProgram, "tex0");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map.baseTex);
		glUniform1i(tex0Address, 0);

		// Direct Light
		GLuint dirlightPosLoc = glGetUniformLocation(shader.shaderProgram, "dirlightPos");
		glUniform3fv(dirlightPosLoc, 1, glm::value_ptr(direct.light_pos));

		GLuint dirlightColorLoc = glGetUniformLocation(shader.shaderProgram, "dirlightColor");
		glUniform3fv(dirlightColorLoc, 1, glm::value_ptr(direct.light_color));

		GLuint dirambientStrLoc = glGetUniformLocation(shader.shaderProgram, "dirambientStr");
		glUniform1f(dirambientStrLoc, direct.ambient_str);

		GLuint dirintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "dirintensityStr");
		glUniform1f(dirintensityStrLoc, direct.intensity);

		// Point Light
		GLuint pntlightPosLoc = glGetUniformLocation(shader.shaderProgram, "pntlightPos");
		glUniform3fv(pntlightPosLoc, 1, glm::value_ptr(point.light_pos));

		GLuint pntlightColorLoc = glGetUniformLocation(shader.shaderProgram, "pntlightColor");
		glUniform3fv(pntlightColorLoc, 1, glm::value_ptr(point.light_color));

		GLuint pntambientStrLoc = glGetUniformLocation(shader.shaderProgram, "pntambientStr");
		glUniform1f(pntambientStrLoc, point.ambient_str);

		GLuint pntintensityStrLoc = glGetUniformLocation(shader.shaderProgram, "pntintensityStr");
		glUniform1f(pntintensityStrLoc, point.intensity);

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

const float skyboxVertices[24] = {
	-1.f, -1.f, 1.f, //0
	1.f, -1.f, 1.f,  //1
	1.f, -1.f, -1.f, //2
	-1.f, -1.f, -1.f,//3
	-1.f, 1.f, 1.f,  //4
	1.f, 1.f, 1.f,   //5
	1.f, 1.f, -1.f,  //6
	-1.f, 1.f, -1.f  //7
};

const unsigned int skyboxIndices[36] = {
		1,2,6,
		6,5,1,

		0,4,7,
		7,3,0,

		4,5,6,
		6,7,4,

		0,3,2,
		2,1,0,

		0,1,5,
		5,4,0,

		3,7,6,
		6,2,3
};

class SkyBox
{
public:
	GLuint VAO, VBO, EBO;

	// Setup Buffers
	SkyBox()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
	}

	// Load Cubemap Textures
	GLuint loadTextures()
	{
		GLuint texture;
		std::string faces[]{
			"Skybox/water_lf.jpg",
			"Skybox/water_rt.jpg",
			"Skybox/water_up.jpg",
			"Skybox/water_dn.jpg",
			"Skybox/water_ft.jpg",
			"Skybox/water_bk.jpg"
		};

		glGenTextures(1, &texture);
		;
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		for (unsigned int i = 0; i < 6; i++) {
			int w, h, skyCChannel;
			stbi_set_flip_vertically_on_load(false);

			unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &skyCChannel, 4);

			if (data) {

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
		}

		stbi_set_flip_vertically_on_load(true);
		return texture;
	}

	// Draw SkyBox
	void draw(MyShader shader, GLuint texture, mat4 projection, mat4 view)
	{
		// Shader Program
		shader.activate();

		// Adjust Depth
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		// Clean View Matrix
		mat4 skyView = mat4(mat3(view));

		// View & Projection Matrices
		GLuint skyboxViewLoc = glGetUniformLocation(shader.shaderProgram, "view");
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, value_ptr(skyView));

		GLuint skyboxProjectLoc = glGetUniformLocation(shader.shaderProgram, "project");
		glUniformMatrix4fv(skyboxProjectLoc, 1, GL_FALSE, value_ptr(projection));

		// Draw
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// Readjust Depth
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}
};

// // // // // // // // // // // // // // // // // // // // // // // // // 

/*	Others Materials
*
	https://antongerdelan.net/colour/ :: float vector color picker
*/



