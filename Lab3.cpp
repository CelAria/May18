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
const GLuint WIDTH = 800, HEIGHT = 800;
const GLfloat CAMERA_MOVEMENT_STEP = 0.1f;
const GLfloat LIGHT_MOVEMENT_STEP = 1.0f; 
const float TEDDY_BEAR_SCALE_STEP = 0.1f;

bool lbutton_down;   // boolean for left mouse button 
float change = 0.1f; // initial value for scale 

GLuint shaderProgram;
GLuint VAO, VBO, EBO;


struct vertexdata {
	GLfloat x, y, z;
	GLfloat	nx, ny, nz;
};

// declaration of interleaved master array which contains all of the indices and normals 

std::vector<vertexdata> masterarray;

glm::vec3 camera_position(0.0f, 3.0f, 5.0f);
glm::vec3 camera_direction(0.0, -3.0, -5.0);
glm::vec3 camera_up = glm::vec3(0, 1, 0);
glm::vec3 light_left = glm::vec3(1, 0, 0);
glm::vec3 light_right = glm::vec3(1, 0, 0);

glm::vec3 triangle_scale = glm::vec3(0.2f);


glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::vec3 light_position(0.0f, 4.0f, 0.0f);
glm::mat4 lightview_matrix = glm::lookAt(glm::vec3(light_position), glm::vec3(0.0f, 0.0f, 0.0f), camera_up);
glm::mat4 light_matrix = projection_matrix * lightview_matrix; // matrix for view from light source for shadow map



void update_camera() {
	view_matrix = glm::lookAt(camera_position, //camera positioned here
		(camera_position + camera_direction), //looks at origin
		camera_up); //up vector

}

void update_light() {
	view_matrix = lightview_matrix;
		

}

void print_vec3(glm::vec3 input) {

	cout << input.x << " " << input.y << " " << input.z << endl;

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)					// close window if esc pressed
		glfwSetWindowShouldClose(window, GL_TRUE);

	//camera movement callback functions 

	//if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)										//move camera position left
	//	camera_position -= glm::cross(camera_direction, camera_up)* CAMERA_MOVEMENT_STEP;
	//if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	//	camera_position += glm::cross(camera_direction, camera_up)* CAMERA_MOVEMENT_STEP; //move camera position right 

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)										//move camera position left
		camera_position.x -= 1.0f;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		camera_position.x += 1.0f;


	if (key == GLFW_KEY_UP && action == GLFW_PRESS)						
		camera_position -= glm::cross(camera_direction, glm::cross(camera_direction, camera_up)* CAMERA_MOVEMENT_STEP);
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		camera_position += glm::cross(camera_direction, glm::cross(camera_direction, camera_up)* CAMERA_MOVEMENT_STEP);

	if (key == GLFW_KEY_T && action == GLFW_PRESS)						 // move camera up and down 
		camera_position.y += 0.1f; 
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		camera_position.y -= 0.1f;

	//if (key == GLFW_KEY_G && action == GLFW_PRESS)						//move camera backward
	//	camera_position += camera_direction;
	//if (key == GLFW_KEY_H && action == GLFW_PRESS)						//move camera forward
	//	camera_position -= camera_direction;

	if (key == GLFW_KEY_G && action == GLFW_PRESS)						//move camera backward
		camera_position.z += 1.0f;
	if (key == GLFW_KEY_H && action == GLFW_PRESS)						//move camera forward
		camera_position.z -= 1.0f; 


	if (key == GLFW_KEY_J && action == GLFW_PRESS)						//rotate camera 
		camera_direction += 0.1f*glm::cross(camera_direction, camera_up);
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
		camera_direction -= 0.1f*glm::cross(camera_direction, camera_up);


	// light movement callback functions 

	if (key == GLFW_KEY_A && action == GLFW_PRESS)						 // move light up and down 
		light_position.y += 1.0f;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		light_position.y -= 1.0f;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)									//move light position left
		light_position.x += 1.0f;
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		light_position.x -= 1.0f;

	update_camera();
	update_light();

	// print out changed for debugging 
	print_vec3(camera_position);
	print_vec3(light_position);
}


// Is called whenever user makes mouse scroll with left button actions 
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (lbutton_down) {
		// drag happening here 

		cout << "Cursor Position at (" << xpos << " : " << ypos << endl;
		change = 0.001f* ypos;
		cout << "change position is" << change << endl;
		
	}
}

void render();

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Object", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Set the required callback functions
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);// enable keycallback
	glfwSetMouseButtonCallback(window, mouse_button_callback);	// enable mousebutton callback
	glfwSetCursorPosCallback(window, cursor_position_callback); //drag callback 
	

	// tell GLFW To capture the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	
	//declare area of projection of model 
	float near_plane = 0.1f, far_plane = 1000.0f;
	projection_matrix = glm::perspective(100.0f, (GLfloat)width / (GLfloat)height, near_plane, far_plane);
	
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
	shaderProgram = glCreateProgram();
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

	//set up vertex data and buffers and configure vertex attributes
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	std::vector<GLfloat> normals;
	std::vector<GLuint> normalindices;

	//3 vertices per triangle (1 normal) every triangle vertex has x, y, z and normals for these
	loadOBJ("scene.obj", vertices, indices, normals, normalindices); //read the vertices and indices from the teddy.obj file


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

	
	//plane VAO 

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO); //index buffer
						   
	// set shadow buffer size to size of window (resolution of depth map) 
	const unsigned int SHADOW_WIDTH = WIDTH, SHADOW_HEIGHT = HEIGHT;
	
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	 // Always check that our framebuffer is ok
 if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
 return false;


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glBufferData(GL_ARRAY_BUFFER, masterarray.size() * sizeof(vertexdata), &masterarray.front(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexdata), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexdata), (GLvoid*)offsetof(vertexdata, nx));  
 
	glEnableVertexAttribArray(0);			//position 
	glEnableVertexAttribArray(1);			//normal

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind


	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	
	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		
		// Render shadow maps 

		glClear(GL_DEPTH_BUFFER_BIT);
		//attach depth buffer to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		

		// shadow depth map 
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);

		lightview_matrix = glm::lookAt(light_position, //light positioned here
			glm::vec3(0.0f, 0.0f, 0.0f), //looks at origin (should be light_direction)
			camera_up); //up vector
		view_matrix = lightview_matrix;

		glDepthMask(GL_FALSE);

		render(); // render from pov of light (depthmap) 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//render scene with regular depth map
		glViewport(0, 0, width, height);
		glActiveTexture(GL_TEXTURE0); // use first texture spot in fragment shader 
		glBindTexture(GL_TEXTURE_2D, depthMap);		// texture filled by pov light depthmap use for render

		
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		//clear color buffer and depth buffer 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);					//use GL LESS hidden surface removal 
									
		update_camera();


		render(); // render with depthmap of shadows
		glBindTexture(GL_TEXTURE_2D, 0);		


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void render() {

	
	// resize 
	triangle_scale = glm::vec3(change);

	glm::mat4 model_matrix;
	model_matrix = glm::scale(model_matrix, triangle_scale);

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint lightpositionLoc = glGetUniformLocation(shaderProgram, "light_position");
	GLuint camerapositionLoc = glGetUniformLocation(shaderProgram, "camera_position");
	GLuint lightviewMatrixLoc = glGetUniformLocation(shaderProgram, "lightview_matrix");
	GLuint lightMatrixLoc = glGetUniformLocation(shaderProgram, "light_matrix");

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(lightviewMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightview_matrix));
	glUniformMatrix4fv(lightMatrixLoc, 1, GL_FALSE, glm::value_ptr(light_matrix));
	glUniform3fv(lightpositionLoc, 1, glm::value_ptr(light_position));
	glUniform3fv(camerapositionLoc, 1, glm::value_ptr(camera_position));


	glBindVertexArray(VAO);


	glDrawArrays(
		GL_TRIANGLES,// mode
		0,
		masterarray.size()    // count

	);

	glBindVertexArray(0);
}