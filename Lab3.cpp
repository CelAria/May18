//Lab 2
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "glew\glew.h"	// include GL Extension Wrangler
#include "glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm\glm.hpp"
#include "glm\gtc/matrix_transform.hpp"
#include "glm\gtc/type_ptr.hpp"

#include "objloader.hpp"

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
const GLfloat CAMERA_MOVEMENT_STEP = 5;
const float TEDDY_BEAR_SCALE_STEP = 0.1f;

glm::vec3 camera_position(0.0f, -1.0f, 1.0f);
glm::vec3 camera_direction(0.0, 1.0, -1.0);
glm::vec3 camera_up = glm::vec3(0, 1, 0);
glm::vec3 triangle_scale = glm::vec3(0.1f);
glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::vec3 light_position(0.0f, 10.0f, 5.0f);

void update_camera() {
	view_matrix = glm::lookAt(camera_position, //camera positioned here
		camera_position + camera_direction, //looks at origin
		camera_up); //up vector

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		camera_position += glm::cross(camera_direction, camera_up);
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		camera_position -= glm::cross(camera_direction, camera_up);


	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		camera_position += glm::cross(camera_direction, glm::cross(camera_direction, camera_up));
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		camera_position -= glm::cross(camera_direction, glm::cross(camera_direction, camera_up));


	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		camera_position += camera_direction;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		camera_position -= camera_direction;


	if (key == GLFW_KEY_J && action == GLFW_PRESS)
		camera_direction += 0.01f*glm::cross(camera_direction, camera_up);
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
		camera_direction -= 0.01f*glm::cross(camera_direction, camera_up);
	update_camera();
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Teddy Bear", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);

	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	std::vector<GLfloat> normals;
	std::vector<GLuint> normalindices;

	//3 vertices per triangle (1 normal) every triangle vertex has x, y, z and normals for these
	loadOBJ("scene.obj", vertices, indices, normals, normalindices); //read the vertices and indices from the teddy.obj file


	struct vertexdata { 
		GLfloat x,y,z;
		GLfloat	nx, ny, nz; 
	};

	// declaration of interleaved master array which contains all of the indices and normals 

	std::vector<vertexdata> masterarray;

	// check that number of vertex indices is equal to number of normals 
	if (indices.size() != normalindices.size()) {
		cout << "indices is not equal to number of normals" << endl;
		return 0;
	}

	// interleave data of vertex indices and normals into masterarray
	for (int k = 0; k < indices.size(); k++) {

		vertexdata v; 

		v.x = vertices[indices[k] * 3];					//index of x of each vertex
		v.y = vertices[(indices[k] * 3) +1];			//index of y of each vertex 
		v.z = vertices[(indices[k] * 3) + 2];			//index of z of each vertex 
			
		v.nx = normals[normalindices[k]*3];				//normal of x at this vertex 	
		v.ny = normals[(normalindices[k] * 3) +1];		//normal of y at this vertex 
		v.nz = normals[(normalindices[k] * 3) + 2];		//normal of z at this vertex 

		masterarray.push_back(v);

	}

	


	GLuint VAO, VBO, EBO;


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO); //index buffer

						   
				
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glBufferData(GL_ARRAY_BUFFER, masterarray.size() * sizeof(vertexdata), &masterarray.front(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexdata), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexdata), (GLvoid*)offsetof(vertexdata, nx));  
 
	glEnableVertexAttribArray(0);			//position 
	glEnableVertexAttribArray(1);			//normal

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind


	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint lightpositionLoc = glGetUniformLocation(shaderProgram, "light_position");
	GLuint camerapositionLoc = glGetUniformLocation(shaderProgram, "camera_position");


	//glEnable(GL_DEPTH_TEST); // enable depth testing for hidden surface removal

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		update_camera();

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glUniform3fv(lightpositionLoc, 1, glm::value_ptr(light_position));
		glUniform3fv(camerapositionLoc, 1, glm::value_ptr(camera_position));


		glBindVertexArray(VAO);


		glDrawArrays(
			GL_TRIANGLES,// mode
			0,
			masterarray.size()    // count
		
		);

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
