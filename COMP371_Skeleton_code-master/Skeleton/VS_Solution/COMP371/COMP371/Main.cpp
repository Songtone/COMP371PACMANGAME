//example skeleton code
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "objloader.hpp"  //include the object loader

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 600;

glm::vec3 camera_position;
glm::vec3 triangle_scale;
glm::mat4 projection_matrix;

float cameraX = 0.0f;
float cameraZ = 1.0f;
float radius = 5.0f;

float pacmanScale = 1.0f;
float pacmanPosX = 0.0f;
float pacmanPosY = 0.0f;
float pacmanDirection = 0.0;

float dotScale = 1.0f;


float ROTATOR = 0.0f;



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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Load one cube", nullptr, nullptr);
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

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJ("pacman.obj", vertices, normals, UVs); //read the vertices from the pacman.obj file

	GLuint VAO_pacman, VBO_pacman,EBO_pacman;
	glGenVertexArrays(1, &VAO_pacman);
	glGenBuffers(1, &VBO_pacman);
	glGenBuffers(1, &EBO_pacman);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	GLuint vertices_VBO_pacman, normals_VBO_pacman;

	glGenVertexArrays(1, &VAO_pacman);
	glGenBuffers(1, &vertices_VBO_pacman);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_pacman);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO_pacman);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO_pacman);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO_pacman);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	std::vector<glm::vec3> verticesSphere;
	std::vector<glm::vec3> normalsSphere;
	std::vector<glm::vec2> UVsSphere;
	loadOBJ("sphere.obj", verticesSphere, normalsSphere, UVsSphere); //read the vertices from the sphere.obj file

	GLuint VAO_sphere, VBO_sphere, EBO_sphere;
	glGenVertexArrays(1, &VAO_sphere);
	glGenBuffers(1, &VBO_sphere);
	glGenBuffers(1, &EBO_sphere);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	GLuint vertices_VBO_sphere, normals_VBO_sphere;

	glGenVertexArrays(1, &VAO_sphere);
	glGenBuffers(1, &vertices_VBO_sphere);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_sphere);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO_sphere);
	glBufferData(GL_ARRAY_BUFFER, verticesSphere.size() * sizeof(glm::vec3), &verticesSphere.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO_sphere);
	glBufferData(GL_ARRAY_BUFFER, normalsSphere.size() * sizeof(glm::vec3), &normalsSphere.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*glBindVertexArray(0);*/
	//grid horizontal lines
	float gridHorizontalLines[]{
		-2.1,0.1,0, 2.1,0.1,0,
		-2.1,-0.1,0, 2.1,-0.1,0,
		-2.1,-0.3,0, 2.1,-0.3,0,
		-2.1,0.3,0, 2.1,0.3,0,
		-2.1,0.5,0, 2.1,0.5,0,
		-2.1,-0.5,0, 2.1,-0.5,0,
		-2.1,0.7,0, 2.1,0.7,0,
		-2.1,-0.7,0, 2.1,-0.7,0,
		-2.1,0.9,0, 2.1,0.9,0,
		-2.1,-0.9,0, 2.1,-0.9,0,
		-2.1,1.1,0, 2.1,1.1,0,
		-2.1,-1.1,0, 2.1,-1.1,0,
		-2.1,1.3,0, 2.1,1.3,0,
		-2.1,-1.3,0, 2.1,-1.3,0,
		-2.1,1.5,0, 2.1,1.5,0,
		-2.1,-1.5,0, 2.1,-1.5,0,
		-2.1,1.7,0, 2.1,1.7,0,
		-2.1,-1.7,0, 2.1,-1.7,0,
		-2.1,1.9,0, 2.1,1.9,0,
		-2.1,-1.9,0, 2.1,-1.9,0,
		-2.1,2.1,0, 2.1,2.1,0,
		-2.1,-2.1,0, 2.1,-2.1,0,
	};

	GLuint VBO_horiLines, VAO_horiLines;
	glGenVertexArrays(1, &VAO_horiLines);
	glBindVertexArray(VAO_horiLines);

	glGenBuffers(1, &VBO_horiLines);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_horiLines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridHorizontalLines), gridHorizontalLines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//grid vertical lines
	float gridVerticalLines[]{
		0.1,-2.1,0, 0.1,2.1,0,
		-0.1,-2.1,0, -0.1,2.1,0,
		0.3,-2.1,0, 0.3,2.1,0,
		-0.3,-2.1,0, -0.3,2.1,0,
		0.5,-2.1,0, 0.5,2.1,0,
		-0.5,-2.1,0, -0.5,2.1,0,
		0.7,-2.1,0, 0.7,2.1,0,
		-0.7,-2.1,0, -0.7,2.1,0,
		0.9,-2.1,0, 0.9,2.1,0,
		-0.9,-2.1,0, -0.9,2.1,0,
		1.1,-2.1,0, 1.1,2.1,0,
		-1.1,-2.1,0, -1.1,2.1,0,
		1.3,-2.1,0, 1.3,2.1,0,
		-1.3,-2.1,0, -1.3,2.1,0,
		1.5,-2.1,0, 1.5,2.1,0,
		-1.5,-2.1,0, -1.5,2.1,0,
		1.7,-2.1,0, 1.7,2.1,0,
		-1.7,-2.1,0, -1.7,2.1,0,
		1.9,-2.1,0, 1.9,2.1,0,
		-1.9,-2.1,0, -1.9,2.1,0,
		2.1,-2.1,0, 2.1,2.1,0,
		-2.1,-2.1,0, -2.1,2.1,0,
		

	};

	GLuint VBO_vertLines, VAO_vertLines;
	glGenVertexArrays(1, &VAO_vertLines);
	glBindVertexArray(VAO_vertLines);

	glGenBuffers(1, &VBO_vertLines);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertLines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridVerticalLines), gridVerticalLines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//three lines that represent the three coordinate axis at origin
	float verticesAxis[]{
		0,0,0, 1,0,0,
		0,0,0, 0,1,0,
		0,0,0, 0,0,1
	};
	/*float verticeColors[]{
		1,0,0,
		1,0,0,
		0,1,0,
		0,1,0,
		0,0,1,
		0,0,1
	};*/

	GLuint VBO_axis, VBO_colors,VAO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glBindVertexArray(VAO_axis);

	glGenBuffers(1, &VBO_axis);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxis), verticesAxis, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*glGenBuffers(1, &VBO_colors);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticeColors), verticeColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);*/

	glBindVertexArray(0);


	triangle_scale = glm::vec3(0.001f);
	glm::vec3 dot_original_scale = glm::vec3(0.01f);

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");

	GLuint transformLoc1 = glGetUniformLocation(shaderProgram, "trimodel");
	

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.4f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Constant vectors
		glm::vec3 center(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::vec3 eye(cameraX, 0.0f, cameraZ);

		//glm::mat4 r_m = glm::rotate(glm::mat4(1.0f), ROTATOR, glm::vec3(0, 1, 0))
			//eye + center normalize


		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(eye, center, up);

		glm::mat4 model_matrix2;
		model_matrix2 = glm::scale(model_matrix2, 200.0f * triangle_scale);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix2));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


		//The grid squares are 0.04 by 0.04 
		//produces the horizontal lines
		glBindVertexArray(VAO_horiLines);
		glDrawArrays(GL_LINES, 0, 46);
		glBindVertexArray(0);

		//produces the vertical lines
		glBindVertexArray(VAO_vertLines);
		glDrawArrays(GL_LINES, 0, 46);
		glBindVertexArray(0);


		//rendering the axis
		glBindVertexArray(VAO_axis);
		glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);
		
		glm::vec3 pacmanPosition = { pacmanPosX, pacmanPosY, 0.0f };//define pacman original position
		glm::mat4 model_pacman; //defines the model_matrix
		glm::mat4 identity_matrix_pacman(1.0f);
		glm::mat4 pacmanScaled = glm::scale(model_pacman, pacmanScale*triangle_scale);//scale pacman object
		glm::mat4 pacmanTranslate = glm::translate(model_pacman, pacmanPosition);//translate pacman
		glm::mat4 pacmanRotationY = glm::rotate(model_pacman,glm::radians(0.0f),glm::vec3(0.0f,1.0f,0.0f));//rotate on Y
		model_pacman = pacmanTranslate * pacmanScaled * pacmanRotationY * identity_matrix_pacman; // trans * scale* rot * identity
		glm::mat4 pacmanLook = glm::rotate(model_pacman, glm::radians(pacmanDirection), glm::vec3(0.0f, 0.0f, 1.0f));//will rotate where pacman looks at
		model_pacman = pacmanLook * identity_matrix_pacman;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_pacman)); //pass model_pacman to shader 
		//rendering pacman
		glBindVertexArray(VAO_pacman);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
		

		glm::vec3 dotPosition = { 0.2f,0.4f,0.0f };
		glm::mat4 model_dot;
		glm::mat4 identity_matrix_dot(1.0f);
		glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
		glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition);
		model_dot = dotTranslate * dotScaled *identity_matrix_dot;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
		//render the dots
		glBindVertexArray(VAO_sphere);
		glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	std::cout << key << std::endl;
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	};
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	};
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	};
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		//cameraX = cameraX + 0.5f;
		//cameraX += radius;
		//cameraZ = cos(glfwGetTime())*radius;
	
	};
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		//cameraX -= radius;
		//cameraZ = -cos(glfwGetTime())*radius;
		
	};
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		cameraZ = cameraZ - 0.5f;
	};
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		cameraZ = cameraZ + 0.5f;
	};
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		pacmanScale += 0.2f;
	};
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		pacmanScale -= 0.2f;
	};
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (pacmanPosY <= 0.36f) {
			pacmanPosY += 0.04f;
			pacmanDirection = 90;
		};
	};
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (pacmanPosY >= -0.36f) {
			pacmanPosY -= 0.04f;
			pacmanDirection = 270;
		};
	};
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		if (pacmanPosX <= 0.36f) {
			pacmanPosX += 0.04f;
			pacmanDirection = 0;
		};
	};
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		if (pacmanPosY >= -0.36f) {
			pacmanPosX -= 0.04f;
			pacmanDirection = 180;
		};
	};
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
};