//example skeleton code
//used http://www.glfw.org/docs/latest/input_guide.html, http://learnopengl.com/ and TA for help on coding
//modified from http://learnopengl.com/

#include "stdafx.h"
#include <time.h>
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


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::vec3 camera_position;
glm::vec3 triangle_scale;
glm::mat4 projection_matrix;

//set camera original position
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 1.0f;
float panning = 0.0f;

//pacman variables
float pacmanScale = 1.0f;
float pacmanPosX = 0.02f;
float pacmanPosY = 0.02f;
float pacmanDirection = 0.0;

//dots variables
bool dotNotEaten0 = true;//booleans to see if the dot has been eaten or not
bool dotNotEaten1 = true;
bool dotNotEaten2 = true;
bool dotNotEaten3 = true;
bool dotNotEaten4 = true;
bool dotNotEaten5 = true;
float dotScale = 1.0f;
glm::vec3 dotPosition[6];

//ghost variables
float ghostScale = 1.0f;//scale for the ghost
int ghostMovement1 = 0;//initializing the original movement of ghost counter
int ghostMovement2 = 0;
int ghostMovement3 = 0;
int ghostMovement4 = 0;
glm::vec3 ghostPosition[4];


float getXPosition();//get the random numbers for the positions of the dots and pacman
float getYPosition();

int yPosition; //for random numbers
int xPosition; //for random numbers

float rangeX;//distance between pacman and a sphere
float rangeY;

//to render the different polygons of the objects
int renderTriangles = 0;
int renderLines = 0;
int renderPoints = 0;

//used in the mouse function
bool allowZoom = false;
bool allowPan = false;
bool allowTilt = false;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
float xoffset;
float yoffset;

//functions used
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void resetGame();//function to reset the game


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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_callback);

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
	string fragment_shader_path = "fragment.shader.shader";
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

	//pacman object
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	loadOBJ("pacman.obj", vertices, normals, UVs); //read the vertices from the pacman.obj file

	GLuint VAO_pacman, VBO_pacman, EBO_pacman;
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
	//the teapot object
	std::vector<glm::vec3> verticesTeapot;
	std::vector<glm::vec3> normalsTeapot;
	std::vector<glm::vec2> UVsTeapot;
	loadOBJ("teapot.obj", verticesTeapot, normalsTeapot, UVsTeapot); //read the vertices from the pacman.obj file

	GLuint VAO_teapot, VBO_teapot, EBO_teapot;
	glGenVertexArrays(1, &VAO_teapot);
	glGenBuffers(1, &VBO_teapot);
	glGenBuffers(1, &EBO_teapot);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	GLuint vertices_VBO_teapot, normals_VBO_teapot;

	glGenVertexArrays(1, &VAO_teapot);
	glGenBuffers(1, &vertices_VBO_teapot);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_teapot);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO_teapot);
	glBufferData(GL_ARRAY_BUFFER, verticesTeapot.size() * sizeof(glm::vec3), &verticesTeapot.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO_teapot);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO_teapot);
	glBufferData(GL_ARRAY_BUFFER, normalsTeapot.size() * sizeof(glm::vec3), &normalsTeapot.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

						  //start of the sphere objects
	std::vector<glm::vec3> verticesSphere;
	std::vector<glm::vec3> normalsSphere;
	std::vector<glm::vec2> UVsSphere;
	loadOBJ("sphere.obj", verticesSphere, normalsSphere, UVsSphere); //read the vertices from the sphere.obj file


	for (int i = 0; i <= 5; i++) {//this will create random positions for the food
		dotPosition[i] = { getXPosition(),getYPosition(),0.0f };

	}


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


	std::vector<glm::vec3> verticesCube;
	std::vector<glm::vec3> normalsCube;
	std::vector<glm::vec2> UVsCube;
	loadOBJ("cube.obj", verticesCube, normalsCube, UVsCube); //read the vertices from the sphere.obj file


	for (int i = 0; i <= 3; i++) {//this will create random positions for the ghosts
		ghostPosition[i] = { getXPosition(),getYPosition(),0.0f };

	}


	GLuint VAO_cube, VBO_cube, EBO_cube;
	glGenVertexArrays(1, &VAO_cube);
	glGenBuffers(1, &VBO_cube);
	glGenBuffers(1, &EBO_cube);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	GLuint vertices_VBO_cube, normals_VBO_cube;

	glGenVertexArrays(1, &VAO_cube);
	glGenBuffers(1, &vertices_VBO_cube);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_cube);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, verticesCube.size() * sizeof(glm::vec3), &verticesCube.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO_cube);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, normalsCube.size() * sizeof(glm::vec3), &normalsCube.front(), GL_STATIC_DRAW);
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


	GLuint VBO_axis, VBO_colors, VAO_axis;
	glGenVertexArrays(1, &VAO_axis);
	glBindVertexArray(VAO_axis);

	glGenBuffers(1, &VBO_axis);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxis), verticesAxis, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	glBindVertexArray(0);


	triangle_scale = glm::vec3(0.001f);
	glm::vec3 pacman_original_scale = glm::vec3(0.001f);
	glm::vec3 dot_original_scale = glm::vec3(0.01f);
	glm::vec3 ghost_original_scale = glm::vec3(0.01f);
	


	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");

	GLuint transformLoc1 = glGetUniformLocation(shaderProgram, "trimodel");

	GLuint object_type_loc = glGetUniformLocation(shaderProgram, "object_type");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Constant vectors
		//glm::vec3 center(pacmanPosX, pacmanPosY, 0.0f);//camera focuses on pacman
		//glm::vec3 up(pacmanPosX, 1.0f, 0.0f);//camera focuses on pacman
		glm::vec3 center(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::vec3 eye(cameraX, cameraY, cameraZ);
		//camera model
		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(eye, center, up);

		//to adjust camera position when using the mouse cursor
		view_matrix = glm::translate(view_matrix, glm::vec3(panning, 0.0f, 0.0f));
		
		glm::mat4 model_grid;
		model_grid = glm::scale(model_grid, 200.0f * triangle_scale);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_grid));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


		//The grid squares are 0.04 by 0.04 
		//produces the horizontal lines
		glUniform1i(object_type_loc, 0);
		glBindVertexArray(VAO_horiLines);
		glDrawArrays(GL_LINES, 0, 44);
		glBindVertexArray(0);

		//produces the vertical lines
		glUniform1i(object_type_loc, 0);
		glBindVertexArray(VAO_vertLines);
		glDrawArrays(GL_LINES, 0, 44);
		glBindVertexArray(0);

		glUniform1i(object_type_loc, 2);
		//rendering the axis
		glBindVertexArray(VAO_axis);
		glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);


		//pacman and teapot section
		glUniform1i(object_type_loc, 3);
		glm::vec3 pacmanPosition = { pacmanPosX, pacmanPosY, 0.0f };//define pacman/teapot original position
		glm::mat4 model_pacman; //defines the model_matrix
		glm::mat4 identity_matrix_pacman(1.0f);
		//glm::mat4 pacmanScaled = glm::scale(model_pacman, pacmanScale*20.0f*pacman_original_scale);//teapot scale
		glm::mat4 pacmanScaled = glm::scale(model_pacman, pacmanScale*pacman_original_scale);//scale pacman object
		glm::mat4 pacmanTranslate = glm::translate(model_pacman, pacmanPosition);//translate pacman
		glm::mat4 pacmanRotationY = glm::rotate(model_pacman, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));//rotate on Y
		model_pacman = pacmanTranslate * pacmanScaled * pacmanRotationY * identity_matrix_pacman; // trans * scale* rot * identity
		glm::mat4 pacmanLook = glm::rotate(model_pacman, glm::radians(pacmanDirection), glm::vec3(0.0f, 0.0f, 1.0f));//will rotate where pacman looks at
		model_pacman = pacmanLook * identity_matrix_pacman;


		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_pacman)); //pass model_pacman to shader 
								 //render teapot
		 //glBindVertexArray(VAO_teapot);
		 //glDrawArrays(GL_TRIANGLES, 0, verticesTeapot.size());
								//rendering pacman
		glBindVertexArray(VAO_pacman);
		if (renderTriangles == 0) {
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		else if (renderLines == 0) {
			glDrawArrays(GL_LINES, 0, vertices.size());
		}
		else if (renderPoints == 0) {
			glDrawArrays(GL_POINTS, 0, vertices.size());
		}
		glBindVertexArray(0);

		//begin of dots section

		if (dotNotEaten0 == true) {//if not true, that means the dot has been eaten and will no longer appear
			rangeX = dotPosition[0].x - pacmanPosition.x;
			rangeY = dotPosition[0].y - pacmanPosition.y;
			if (abs(rangeX) < 0.001f && abs(rangeY) < 0.001f) {
				dotNotEaten0 = false;

			}
			else {
				glUniform1i(object_type_loc, 1);
				//glm::vec3 dotPosition = { dotPosX,dotPosY,0.0f };
				glm::mat4 model_dot;
				glm::mat4 identity_matrix_dot(1.0f);
				glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
				glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition[0]);
				model_dot = dotTranslate * dotScaled *identity_matrix_dot;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
																						  //render the dots
				glBindVertexArray(VAO_sphere);
				if (renderTriangles == 0) {
					glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
				}
				else if (renderLines == 0) {
					glDrawArrays(GL_LINES, 0, verticesSphere.size());
				}
				else if (renderPoints == 0) {
					glDrawArrays(GL_POINTS, 0, verticesSphere.size());
				}
				glBindVertexArray(0);


			}
		}
		if (dotNotEaten1 == true) {//if not true, that means the dot has been eaten and will no longer appear
			rangeX = dotPosition[1].x - pacmanPosition.x;
			rangeY = dotPosition[1].y - pacmanPosition.y;
			if (abs(rangeX) < 0.001f && abs(rangeY) < 0.001f) {
				dotNotEaten1 = false;

			}
			else {
				glUniform1i(object_type_loc, 1);
				//glm::vec3 dotPosition = { dotPosX,dotPosY,0.0f };
				glm::mat4 model_dot;
				glm::mat4 identity_matrix_dot(1.0f);
				glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
				glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition[1]);
				model_dot = dotTranslate * dotScaled *identity_matrix_dot;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
																						  //render the dots
				glBindVertexArray(VAO_sphere);
				if (renderTriangles == 0) {
					glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
				}
				else if (renderLines == 0) {
					glDrawArrays(GL_LINES, 0, verticesSphere.size());
				}
				else if (renderPoints == 0) {
					glDrawArrays(GL_POINTS, 0, verticesSphere.size());
				}
				glBindVertexArray(0);


			}
		}
		if (dotNotEaten2 == true) {//if not true, that means the dot has been eaten and will no longer appear
			rangeX = dotPosition[2].x - pacmanPosition.x;
			rangeY = dotPosition[2].y - pacmanPosition.y;
			if (abs(rangeX) < 0.001f && abs(rangeY) < 0.001f) {
				dotNotEaten2 = false;

			}
			else {
				glUniform1i(object_type_loc, 1);
				//glm::vec3 dotPosition = { dotPosX,dotPosY,0.0f };
				glm::mat4 model_dot;
				glm::mat4 identity_matrix_dot(1.0f);
				glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
				glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition[2]);
				model_dot = dotTranslate * dotScaled *identity_matrix_dot;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
																						  //render the dots
				glBindVertexArray(VAO_sphere);
				if (renderTriangles == 0) {
					glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
				}
				else if (renderLines == 0) {
					glDrawArrays(GL_LINES, 0, verticesSphere.size());
				}
				else if (renderPoints == 0) {
					glDrawArrays(GL_POINTS, 0, verticesSphere.size());
				}
				glBindVertexArray(0);


			}
		}
		if (dotNotEaten3 == true) {//if not true, that means the dot has been eaten and will no longer appear
			rangeX = dotPosition[3].x - pacmanPosition.x;
			rangeY = dotPosition[3].y - pacmanPosition.y;
			if (abs(rangeX) < 0.001f && abs(rangeY) < 0.001f) {
				dotNotEaten3 = false;

			}
			else {
				glUniform1i(object_type_loc, 1);
				//glm::vec3 dotPosition = { dotPosX,dotPosY,0.0f };
				glm::mat4 model_dot;
				glm::mat4 identity_matrix_dot(1.0f);
				glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
				glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition[3]);
				model_dot = dotTranslate * dotScaled *identity_matrix_dot;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
																						  //render the dots
				glBindVertexArray(VAO_sphere);
				if (renderTriangles == 0) {
					glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
				}
				else if (renderLines == 0) {
					glDrawArrays(GL_LINES, 0, verticesSphere.size());
				}
				else if (renderPoints == 0) {
					glDrawArrays(GL_POINTS, 0, verticesSphere.size());
				}
				glBindVertexArray(0);


			}
		}
		if (dotNotEaten4 == true) {//if not true, that means the dot has been eaten and will no longer appear
			rangeX = dotPosition[4].x - pacmanPosition.x;
			rangeY = dotPosition[4].y - pacmanPosition.y;
			if (abs(rangeX) < 0.001f && abs(rangeY) < 0.001f) {
				dotNotEaten4 = false;

			}
			else {
				glUniform1i(object_type_loc, 1);
				//glm::vec3 dotPosition = { dotPosX,dotPosY,0.0f };
				glm::mat4 model_dot;
				glm::mat4 identity_matrix_dot(1.0f);
				glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
				glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition[4]);
				model_dot = dotTranslate * dotScaled *identity_matrix_dot;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
																						  //render the dots
				glBindVertexArray(VAO_sphere);
				if (renderTriangles == 0) {
					glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
				}
				else if (renderLines == 0) {
					glDrawArrays(GL_LINES, 0, verticesSphere.size());
				}
				else if (renderPoints == 0) {
					glDrawArrays(GL_POINTS, 0, verticesSphere.size());
				}
				glBindVertexArray(0);


			}
		}if (dotNotEaten5 == true) {//if not true, that means the dot has been eaten and will no longer appear
			rangeX = dotPosition[5].x - pacmanPosition.x;
			rangeY = dotPosition[5].y - pacmanPosition.y;
			if (abs(rangeX) < 0.001f && abs(rangeY) < 0.001f) {
				dotNotEaten5 = false;

			}
			else {
				glUniform1i(object_type_loc, 1);
				//glm::vec3 dotPosition = { dotPosX,dotPosY,0.0f };
				glm::mat4 model_dot;
				glm::mat4 identity_matrix_dot(1.0f);
				glm::mat4 dotScaled = glm::scale(model_dot, dotScale*dot_original_scale);//scale dot object
				glm::mat4 dotTranslate = glm::translate(model_dot, dotPosition[5]);
				model_dot = dotTranslate * dotScaled *identity_matrix_dot;

				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_dot)); // pass model_dot to shader
																						  //render the dots
				glBindVertexArray(VAO_sphere);
				if (renderTriangles == 0) {
					glDrawArrays(GL_TRIANGLES, 0, verticesSphere.size());
				}
				else if (renderLines == 0) {
					glDrawArrays(GL_LINES, 0, verticesSphere.size());
				}
				else if (renderPoints == 0) {
					glDrawArrays(GL_POINTS, 0, verticesSphere.size());
				}
				glBindVertexArray(0);


			}
		}


		//This will check if all the dots have been eaten, if yes then the game will reset itself
		if (dotNotEaten0 == false && dotNotEaten1 == false && dotNotEaten2 == false && dotNotEaten3 == false && dotNotEaten4 == false && dotNotEaten5 == false) {
			cout << "You Win!" << endl;
			resetGame();
		}

															//ghost starts here
		
		//ghost number 1
		if (ghostMovement1 == 10) {//to control the speed of the ghosts
			if (abs(ghostPosition[0].x - pacmanPosX) < 0.0000001f && abs(ghostPosition[0].y - pacmanPosY) < 0.0000001f) {
				resetGame();//once the ghost touches pacman, the game will restart
			}
			else if (abs(ghostPosition[0].x - pacmanPosX) > 0.000000001f && abs(ghostPosition[0].y - pacmanPosY) > 0.000000001f) {
				if (abs(ghostPosition[0].x - pacmanPosX) > abs(ghostPosition[0].y - pacmanPosY)) {
					if (ghostPosition[0].x < pacmanPosX) {
						ghostPosition[0].x = ghostPosition[0].x + 0.04f;
					}
					else if (ghostPosition[0].x > pacmanPosX) {
						ghostPosition[0].x = ghostPosition[0].x - 0.04f;
					}

				}
				else {
					if (ghostPosition[0].y < pacmanPosY) {
						ghostPosition[0].y = ghostPosition[0].y + 0.04f;
					}
					else if (ghostPosition[0].y > pacmanPosY) {
						ghostPosition[0].y = ghostPosition[0].y - 0.04f;
					}
				}
			}


			ghostMovement1 = 0;
		}
		ghostMovement1++;
		//ghost rendering
		glUniform1i(object_type_loc, 4);
		glm::mat4 model_ghost1;
		glm::mat4 identity_matrix_ghost1(1.0f);
		glm::mat4 ghostScaled1 = glm::scale(model_ghost1, ghostScale*ghost_original_scale);//scale dot object
		glm::mat4 ghostTranslate1 = glm::translate(model_ghost1, ghostPosition[0]);
		model_ghost1 = ghostTranslate1 * ghostScaled1 *identity_matrix_ghost1;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_ghost1)); // pass model_dot to shader
																					//render the dots
		glBindVertexArray(VAO_cube);
		if (renderTriangles == 0) {
			glDrawArrays(GL_TRIANGLES, 0, verticesCube.size());
		}
		else if (renderLines == 0) {
			glDrawArrays(GL_LINES, 0, verticesCube.size());
		}
		else if (renderPoints == 0) {
			glDrawArrays(GL_POINTS, 0, verticesCube.size());
		}
		glBindVertexArray(0);


												//ghost number 2

		if (ghostMovement2 == 20) {//to control the speed of the ghosts
			if (abs(ghostPosition[1].x - pacmanPosX) < 0.0000001f && abs(ghostPosition[1].y - pacmanPosY) < 0.0000001f) {
				resetGame();//once the ghost touches pacman, the game restarts
			}
			else if (abs(ghostPosition[1].x - pacmanPosX) > 0.000000001f && abs(ghostPosition[1].y - pacmanPosY) > 0.000000001f) {
				if (abs(ghostPosition[1].x - pacmanPosX) > abs(ghostPosition[1].y - pacmanPosY)) {
					if (ghostPosition[1].x < pacmanPosX) {
						ghostPosition[1].x = ghostPosition[1].x + 0.04f;
					}
					else if (ghostPosition[1].x > pacmanPosX) {
						ghostPosition[1].x = ghostPosition[1].x - 0.04f;
					}

				}
				else {
					if (ghostPosition[1].y < pacmanPosY) {
						ghostPosition[1].y = ghostPosition[1].y + 0.04f;
					}
					else if (ghostPosition[1].y > pacmanPosY) {
						ghostPosition[1].y = ghostPosition[1].y - 0.04f;
					}
				}
			}


			ghostMovement2 = 0;
		}
		ghostMovement2++;
		//ghost rendering
		glUniform1i(object_type_loc, 5);
		glm::mat4 model_ghost2;
		glm::mat4 identity_matrix_ghost2(1.0f);
		glm::mat4 ghostScaled2 = glm::scale(model_ghost2, ghostScale*ghost_original_scale);//scale dot object
		glm::mat4 ghostTranslate2 = glm::translate(model_ghost2, ghostPosition[1]);
		model_ghost2 = ghostTranslate2 * ghostScaled2 *identity_matrix_ghost2;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_ghost2)); // pass model_dot to shader
																					//render the dots
		glBindVertexArray(VAO_cube);
		if (renderTriangles == 0) {
			glDrawArrays(GL_TRIANGLES, 0, verticesCube.size());
		}
		else if (renderLines == 0) {
			glDrawArrays(GL_LINES, 0, verticesCube.size());
		}
		else if (renderPoints == 0) {
			glDrawArrays(GL_POINTS, 0, verticesCube.size());
		}
		glBindVertexArray(0);

												//ghost number 3	


		if (ghostMovement3 == 30) {//to control the speed of the ghosts
			if (abs(ghostPosition[2].x - pacmanPosX) < 0.0000001f && abs(ghostPosition[2].y - pacmanPosY) < 0.0000001f) {
				resetGame();//once the ghost touches pacman, the game will restart
			}
			else if (abs(ghostPosition[2].x - pacmanPosX) > 0.000000001f && abs(ghostPosition[2].y - pacmanPosY) > 0.000000001f) {
				if (abs(ghostPosition[2].x - pacmanPosX) > abs(ghostPosition[2].y - pacmanPosY)) {
					if (ghostPosition[2].x < pacmanPosX) {
						ghostPosition[2].x = ghostPosition[2].x + 0.04f;
					}
					else if (ghostPosition[2].x > pacmanPosX) {
						ghostPosition[2].x = ghostPosition[2].x - 0.04f;
					}

				}
				else {
					if (ghostPosition[2].y < pacmanPosY) {
						ghostPosition[2].y = ghostPosition[2].y + 0.04f;
					}
					else if (ghostPosition[2].y > pacmanPosY) {
						ghostPosition[2].y = ghostPosition[2].y - 0.04f;
					}
				}
			}


			ghostMovement3 = 0;
		}
		ghostMovement3++;
		//ghost rendering
		glUniform1i(object_type_loc, 6);
		glm::mat4 model_ghost3;
		glm::mat4 identity_matrix_ghost3(1.0f);
		glm::mat4 ghostScaled3 = glm::scale(model_ghost3, ghostScale*ghost_original_scale);//scale dot object
		glm::mat4 ghostTranslate3 = glm::translate(model_ghost3, ghostPosition[2]);
		model_ghost3 = ghostTranslate3 * ghostScaled3 *identity_matrix_ghost3;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_ghost3)); // pass model_dot to shader
																					 //render the dots
		glBindVertexArray(VAO_cube);
		if (renderTriangles == 0) {
			glDrawArrays(GL_TRIANGLES, 0, verticesCube.size());
		}
		else if (renderLines == 0) {
			glDrawArrays(GL_LINES, 0, verticesCube.size());
		}
		else if (renderPoints == 0) {
			glDrawArrays(GL_POINTS, 0, verticesCube.size());
		}
		glBindVertexArray(0);

														//ghost number 4	


		if (ghostMovement4 == 15) {//to control the speed of the ghosts
			if (abs(ghostPosition[3].x - pacmanPosX) < 0.0000001f && abs(ghostPosition[3].y - pacmanPosY) < 0.0000001f) {
				resetGame();//once the ghost touches pacman, the game will restart
			}
			else if (abs(ghostPosition[3].x - pacmanPosX) > 0.000000001f && abs(ghostPosition[3].y - pacmanPosY) > 0.000000001f) {
				if (abs(ghostPosition[3].x - pacmanPosX) > abs(ghostPosition[3].y - pacmanPosY)) {
					if (ghostPosition[3].x < pacmanPosX) {
						ghostPosition[3].x = ghostPosition[3].x + 0.04f;
					}
					else if (ghostPosition[3].x > pacmanPosX) {
						ghostPosition[3].x = ghostPosition[3].x - 0.04f;
					}

				}
				else {
					if (ghostPosition[3].y < pacmanPosY) {
						ghostPosition[3].y = ghostPosition[3].y + 0.04f;
					}
					else if (ghostPosition[3].y > pacmanPosY) {
						ghostPosition[3].y = ghostPosition[3].y - 0.04f;
					}
				}
			}


			ghostMovement4 = 0;
		}
		ghostMovement4++;
		//ghost rendering
		glUniform1i(object_type_loc, 7);
		glm::mat4 model_ghost4;
		glm::mat4 identity_matrix_ghost4(1.0f);
		glm::mat4 ghostScaled4 = glm::scale(model_ghost4, ghostScale*ghost_original_scale);//scale dot object
		glm::mat4 ghostTranslate4 = glm::translate(model_ghost4, ghostPosition[3]);
		model_ghost4 = ghostTranslate4 * ghostScaled4 *identity_matrix_ghost4;

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_ghost4)); // pass model_dot to shader
																					 //render the dots
		glBindVertexArray(VAO_cube);
		if (renderTriangles == 0) {
			glDrawArrays(GL_TRIANGLES, 0, verticesCube.size());
		}
		else if (renderLines == 0) {
			glDrawArrays(GL_LINES, 0, verticesCube.size());
		}
		else if (renderPoints == 0) {
			glDrawArrays(GL_POINTS, 0, verticesCube.size());
		}
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
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {//polygon with lines
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderLines = 0;
		renderPoints = 1;
		renderTriangles = 1;
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {//dot polygon
		//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		renderLines = 1;
		renderPoints = 0;
		renderTriangles = 1;

	}if (key == GLFW_KEY_T && action == GLFW_PRESS) {//dot polygon
		renderLines = 1;
		renderPoints = 1;
		renderTriangles = 0;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		cameraX = cameraX + 0.05f;
		if (cameraX >= 0.0f) {
			cameraZ = cameraZ - 0.02f;
		}
		else {
			cameraZ = cameraZ + 0.02f;
		}
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		cameraX = cameraX - 0.05f;
		if (cameraX <= 0.0f) {
			cameraZ = cameraZ - 0.02f;
		}
		else {
			cameraZ = cameraZ + 0.02f;
		}
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {//move the camera closer to the grid
		cameraY = cameraY + 0.05f;
		if (cameraY >= 0.0f) {
			cameraZ = cameraZ - 0.02f;
		}
		else {
			cameraZ = cameraZ + 0.02f;
		}
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {//move the camera further from the grid
		cameraY = cameraY - 0.05f;
		if (cameraY <= 0.0f) {
			cameraZ = cameraZ - 0.02f;
		}
		else {
			cameraZ = cameraZ + 0.02f;
		}
	}
	if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {//reset the orientation and the position of the world
			panning = 0.0f;
			cameraX = 0.0f;
			cameraY = 0.0f;
			cameraZ = 1.0f;
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {//scale up the objects
		pacmanScale += 0.2f;
		dotScale += 0.2f;
		ghostScale += 0.2f;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {//scale down the objects
		pacmanScale -= 0.2f;
		dotScale -= 0.2f;
		ghostScale -= 0.2f;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {//moving pacman on upwards on the Y axis using W
		if (pacmanPosY <= 0.4f) {
			pacmanPosY += 0.04f;
			pacmanDirection = 90;
		}
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {//moving pacman on downwards on the Y axis using S
		if (pacmanPosY >= -0.4f) {
			pacmanPosY -= 0.04f;
			pacmanDirection = 270;
		}
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {//moving pacman to the right on the X axis using D
		if (pacmanPosX <= 0.4f) {
			pacmanPosX += 0.04f;
			pacmanDirection = 0;
		}
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {//moving pacman to the left on the X axis using A
		if (pacmanPosX >= -0.4f) {
			pacmanPosX -= 0.04f;
			pacmanDirection = 180;
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {//pressing spacebar will reset the game
		pacmanPosX = getXPosition();
		pacmanPosY = getYPosition();
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		resetGame();
	}
	
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	if (allowPan == true) {
		xoffset = xpos - lastX;
		lastX = xpos;
		if (xoffset < 0.0f) {//going left
			panning -= 0.015f;
		}
		else if (xoffset > 0.0f) {//going right
			panning += 0.015f;
		}
	}
	if (allowTilt == true) {
		yoffset = ypos - lastY;
		lastY = ypos;
		xoffset = xpos - lastX;
		lastX = xpos;
		if (yoffset < 0.0f) {//upwards
			if (cameraY >= 0.0f) {
				cameraY += 0.015f;
				cameraZ -= 0.015f;
			}
			else if (cameraY <= 0.0f) {
				cameraY += 0.015f;
				cameraZ += 0.015f;
			}
		}
		else if (yoffset > 0.0f) {//downwards
			if (cameraY >= 0.0f) {
				cameraY -= 0.015f;
				cameraZ += 0.015f;
			}
			else if (cameraY <= 0.0f) {
				cameraY -= 0.015f;
				cameraZ -= 0.015f;
			}
		}if (xoffset < 0.0f) {//left
			if (cameraX >= 0.0f) {
				cameraX -= 0.015f;
				cameraZ += 0.015f;
			}
			else if (cameraY <= 0.0f) {
				cameraX -= 0.015f;
				cameraZ -= 0.015f;
			}
		}
		else if (yoffset > 0.0f) {//right
			if (cameraX >= 0.0f) {
				cameraX += 0.015f;
				cameraZ -= 0.015f;
			}
			else if (cameraY <= 0.0f) {
				cameraX += 0.015f;
				cameraZ += 0.015f;
			}
		}
	}
	if (allowZoom == true) {
		yoffset = ypos - lastY;
		lastY = ypos;
		if (yoffset < 0.0f) {//upwards
			if (cameraZ > 0.1f) {
				cameraZ -= 0.015f;
			}
		}
		else if (yoffset > 0.0f) {//downwards
			cameraZ += 0.015f;
		}
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		allowZoom = true;
		
	}
	else {
		allowZoom = false;
		
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		
		allowTilt = true;
	}
	else {
		allowTilt = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		allowPan = true;
	}
	else {
		allowPan = false;
		
	}

}

//making sure the objects keep their aspect ratio
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
	glViewport(0, 0, width, height);
}
//this function will reset the game. Resets dot position, pacman position, camera position, grid position
void resetGame() {
	
	ghostMovement1 = 0;
	ghostMovement2 = 0;
	ghostMovement3 = 0;
	ghostMovement4 = 0;
	pacmanPosX = getXPosition();
	pacmanPosY = getYPosition();
	for (int i = 0; i <= 5; i++) {//this will create random positions for the food
		dotPosition[i] = { getXPosition(),getYPosition(), 0.0f };
		dotNotEaten0 = true;
		dotNotEaten1 = true;
		dotNotEaten2 = true;
		dotNotEaten3 = true;
		dotNotEaten4 = true;
		dotNotEaten5 = true;

	}
}



float getYPosition() {//will get a random position on the Y axis

	yPosition = rand() % 22 + 1;

	switch (yPosition) {
	case 1:return 0.42f;
		break;
	case 2:return 0.38f;
		break;
	case 3:return 0.34f;
		break;
	case 4:return 0.30f;
		break;
	case 5:return 0.26f;
		break;
	case 6:return 0.22f;
		break;
	case 7:return 0.18f;
		break;
	case 8:return 0.14f;
		break;
	case 9:return 0.10f;
		break;
	case 10:return 0.06f;
		break;
	case 11:return 0.02f;
		break;
	case 12:return -0.02f;
		break;
	case 13:return -0.06f;
		break;
	case 14:return -0.10f;
		break;
	case 15:return -0.14f;
		break;
	case 16:return -0.18f;
		break;
	case 17:return -0.22f;
		break;
	case 18:return -0.26f;
		break;
	case 19:return -0.30f;
		break;
	case 20:return -0.34f;
		break;
	case 21:return -0.38f;
		break;
	case 22:return -0.42f;
		break;

	}

}

float getXPosition() {//will get a random position on the X axis

	xPosition = rand() % 22 + 1;

	switch (xPosition) {
	case 1:return 0.42f;
		break;
	case 2:return 0.38f;
		break;
	case 3:return 0.34f;
		break;
	case 4:return 0.30f;
		break;
	case 5:return 0.26f;
		break;
	case 6:return 0.22f;
		break;
	case 7:return 0.18f;
		break;
	case 8:return 0.14f;
		break;
	case 9:return 0.10f;
		break;
	case 10:return 0.06f;
		break;
	case 11:return 0.02f;
		break;
	case 12:return -0.02f;
		break;
	case 13:return -0.06f;
		break;
	case 14:return -0.10f;
		break;
	case 15:return -0.14f;
		break;
	case 16:return -0.18f;
		break;
	case 17:return -0.22f;
		break;
	case 18:return -0.26f;
		break;
	case 19:return -0.30f;
		break;
	case 20:return -0.34f;
		break;
	case 21:return -0.38f;
		break;
	case 22:return -0.42f;
		break;
	}
}