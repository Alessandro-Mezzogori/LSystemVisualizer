#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <ext.hpp>
#include <filesystem>
#include "lsystem.h"

/*Program Status variables*/
//vertex buffer objects ids
unsigned int vertexArrayObjID[1];
unsigned int vertexBufferObjID[1];
GLint number_of_vertices, program, windowId;

/*show all the saved files*/
void printSavedFilesName() {
	for (const auto &entry : std::filesystem::directory_iterator("saved_files"))
		std::cout << entry.path() << std::endl;
	std::cout << std::endl;
}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
char* loadFile(std::string fname, GLuint &fSize){
	std::ifstream::pos_type size;
	char * memblock;
	std::string text;
	// file read based on example in cplusplus.com tutorial
	std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		fSize = (GLuint)size;
		memblock = new char[fSize];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();
		std::cout << "file " << fname << " loaded" << std::endl;
		text.assign(memblock);
	}
	else{
		std::cout << "Unable to open file " << fname << std::endl;
		memblock = NULL;
	}
	file.close();
	return memblock;
}

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
		delete[] infoLog;
	}

	// should additionally check for OpenGL errors here
}

//min max coords of the cube encompassing all the coords
std::array<std::pair<GLfloat, GLfloat>, 3> getEncasingSquareCoords(const GLfloat *vertices, const GLuint vertices_len) {
	//first is the min coord, right is the max coord
	std::array<std::pair<GLfloat, GLfloat>, 3> minmax_coords = { std::make_pair(vertices[0],vertices[0]),
																std::make_pair(vertices[1],vertices[1]),
																std::make_pair(vertices[2],vertices[2]) };
	for (GLuint i = 3; i < vertices_len; i += 3) {
		//x coord
		if (minmax_coords[0].first > vertices[i])
			minmax_coords[0].first = vertices[i];
		if (minmax_coords[0].second < vertices[i])
			minmax_coords[0].second = vertices[i];
		//y coord
		if (minmax_coords[1].first > vertices[i + 1])
			minmax_coords[1].first = vertices[i + 1];
		if (minmax_coords[1].second < vertices[i + 1])
			minmax_coords[1].second = vertices[i + 1];
		//z coord
		if (minmax_coords[2].first > vertices[i + 2])
			minmax_coords[2].first = vertices[i + 2];
		if (minmax_coords[2].second < vertices[i + 2])
			minmax_coords[2].second = vertices[i + 2];
	}
	return minmax_coords;
}


GLfloat *loadData(std::string filename, GLuint &size) {
	
	GLfloat *vertices = (GLfloat*)loadFile(filename, size);
	if (vertices == NULL)
		return NULL;
	//initialize global variable
	number_of_vertices = size / (3 * sizeof(GLfloat));
	return vertices;
}

//initialization of the buffers
void genBuffers() {
	// Allocate Vertex Array Objects
	glGenVertexArrays(1, vertexArrayObjID);
	// Setup first Vertex Array Object
	glBindVertexArray(vertexArrayObjID[0]);
	glGenBuffers(1, vertexBufferObjID);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void fillBuffers(const GLfloat *vertices, const GLuint vertices_size) {
	glBindVertexArray(vertexArrayObjID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//ViewModelProjection matrix initialization to have it perpendicular to the XY plane, looking at the middle point of the L system
void initMatrices(const GLfloat *vertices, const GLuint size) {
	std::array<std::pair<GLfloat, GLfloat>, 3> minmax_coords = getEncasingSquareCoords(vertices, size / sizeof(GLfloat));

	std::array<float, 3> middle_point = { (minmax_coords[0].first + minmax_coords[0].second) / 2.0f,
					(minmax_coords[1].first + minmax_coords[1].second) / 2.0f,
					(minmax_coords[2].first + minmax_coords[2].second) / 2.0f, };

	std::array<float, 3> max_distances = { (minmax_coords[0].second - minmax_coords[0].first),
					(minmax_coords[1].second - minmax_coords[1].first),
					(minmax_coords[2].second - minmax_coords[2].first), };


 	glm::mat4 ProjectionMatrix = glm::perspective(static_cast<float>(glm::radians(45.0f)), 600.0f / 600.0f, 0.1f, 100.0f);
	glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(middle_point.at(0), middle_point.at(1), 15.0f),
										glm::vec3(middle_point.at(0), middle_point.at(1), middle_point.at(2)),
										glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 ScalingMatrix = glm::mat4(glm::vec4(10.0f / max_distances.at(0), 0.0f, 0.0f, 0.0f),
										glm::vec4(0.0f, 10.0f/max_distances.at(1), 0.0f, 0.0f),
										glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
										glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	glm::mat4 mvp = ProjectionMatrix * ScalingMatrix * ViewMatrix;
	GLint uniMvp = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(uniMvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void initShaders()
{
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	// load shaders & get length of each
	GLuint vlen, flen;
	char vertexShader[] = "minimal.vert";
	char fragShader[] = "minimal.frag";
	char* vs = loadFile(vertexShader, vlen);
	char* fs = loadFile(fragShader, flen);

	const char * vv = vs;
	const char * ff = fs;

	GLint vvlen = vlen;
	GLint fflen = flen;
	glShaderSource(v, 1, &vv, &vvlen);
	glShaderSource(f, 1, &ff, &fflen);

	GLint compiled;

	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cout << "Vertex shader not compiled." << std::endl;
		printShaderInfoLog(v);
	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cout << "Fragment shader not compiled." << std::endl;
		printShaderInfoLog(f);
	}

	GLint p = glCreateProgram();

	glBindAttribLocation(p, 0, "in_Position");
	glBindAttribLocation(p, 1, "in_Color");
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);
	glUseProgram(p);

	delete[] vs; // dont forget to free allocated memory
	delete[] fs; // we allocated this in the loadFile function...
	//global variable
	program = p;
}


void loadLSystem(unsigned int choice, unsigned int numberOfInterations)
{
	GLuint vertices_size = 0;
	std::string filename = "saved_files/" + getLSystemFileName((LSystemCode)choice, numberOfInterations);
	GLfloat *vertices = NULL;
	
	if(choice != 0)
		vertices = loadData(filename, vertices_size);
	
	if(vertices == NULL){ //found no default filename
		//generate data points
		//choise of l system and number of iterations
		lsGenData(choice, numberOfInterations, "saved_files/default.bin");
		vertices = loadData("saved_files/default.bin", vertices_size);
		if (vertices == NULL)
			return;
	}
	fillBuffers(vertices, vertices_size);
	initMatrices(vertices, vertices_size);
	glutPostRedisplay();
}

void loadLSystemFile(std::string filename) {
	GLuint vertices_size = 0;
	GLfloat *vertices = loadData(filename, vertices_size);
	if (vertices == NULL)
		return;

	fillBuffers(vertices, vertices_size);
	initMatrices(vertices, vertices_size);
	glutPostRedisplay();
}

void display()
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindVertexArray(vertexArrayObjID[0]);	// First VAO
	glDrawArrays(GL_LINES, 0, number_of_vertices);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void processInput(std::string *input) {
	auto input_stream = std::istringstream(*input);
	std::string token;

	static std::string current_input_filename = "default.bin";
	static int current_lsystemcode = 15;
	static int current_numberOfIterations = 0;
	static bool drawed = false;
	while (input_stream >> token) {
		if (token == "help" || token == "h"){
			std::cout << std::string(50, '\n');
			std::cout << "To draw one of the possible L-Systems: 'draw' or 'd' (LSYSTEM_CODE N)" << std::endl;
			std::cout << "To save a drawed L-System: 'save (filename | -d)'" << std::endl;
			std::cout << "To load a saved L-System: 'load filename'" << std::endl;
			std::cout << "To list the name of the saved L-System: 'list' or 'ls' (-s | -c)" << std::endl;
			std::cout << "To delete a saved system: 'delete' or 'del' (filename)" << std::endl;
			std::cout << "To quit the program: 'exit' or 'quit'" << std::endl;
			
		}
		else if (token == "list" || token == "ls") {
			std::string tag;
			input_stream >> tag;
			if (!input_stream.fail()) {
				if (tag == "-c" || tag == "-codes")
					printLSystemOptions();
				else if (tag == "-s" || tag == "-saved")
					printSavedFilesName();
			}
			else
				std::cout << "INPUT ERROR: MISSING TAG -s or -c" << std::endl;
		}
		else if (token == "draw" || token == "d") {
			drawed = true;
			//input control
			input_stream >> current_lsystemcode >> current_numberOfIterations;
			if (!input_stream.fail()){
				loadLSystem(current_lsystemcode, current_numberOfIterations);
				current_input_filename = "default.bin";
			}
			else
				std::cout << "INPUT ERROR: INVALID PARAMS" << std::endl;

		
		}
		else if ((token == "save" || token == "-s") && drawed == true) {
			std::string filename;
			input_stream >> filename;
			if (!input_stream.fail()) {	
				if (filename == "-d") {
					filename = getLSystemFileName((LSystemCode)current_lsystemcode, current_numberOfIterations).c_str();
				}

				if (filename.find(".bin") == std::string::npos) {
					std::cout << "INPUT ERROR: FILENAME WITHOUT EXTENSION" << std::endl;
					std::cout << "USING THE DEFAULT EXTENSION .BIN" << std::endl;
					filename.append(".bin");
				}
				std::cout << "Saving default temporary file as " << filename << std::endl;
				std::rename(("saved_files/" + current_input_filename).c_str(), ("saved_files/" + filename).c_str()); //saveLSystem(filename);
				std::cout << "Finished saving " << filename << std::endl;
			}
			else
				std::cout << "INPUT ERROR: MISSING FILENAME TAG" << std::endl;
		}
		else if (token == "load") {
			drawed = false;
			input_stream >> current_input_filename;
			if (!input_stream.fail()) {
				loadLSystemFile("saved_files/" + current_input_filename);
			}
			else
				std::cout << "INPUT ERROR: MISSING FILENAME TAG";
		}
		else if (token == "delete" or token=="del") {
			std::string tag;
			input_stream >> tag;
			if (!input_stream.fail()) {
				if (tag != "default.bin") {
					std::cout << "Deleting " << tag << std::endl;
					int status = remove(("saved_files/" + tag).c_str());
					if (status == 0)
						std::cout << "File deleted successfully" << std::endl;
					else
						std::cout << "ERROR DELETING FILE" << std::endl;
				}
				else
					std::cout << "ERROR: TRYING TO DELETE DEFAULT FILE" << std::endl;
			}
			else
				std::cout << "INPUT ERROR: MISSING FILENAME TAG" << std::endl;
		}
		else if (token == "exit" || token == "quit") {
			glutDestroyWindow(windowId);
			glutLeaveMainLoop();
		}
	}
}

void loopTimer(int value) {
	std::cout << "To get a list of the possible L-Systems and commands: 'help' or 'h'" << std::endl;

	std::string input;
	std::getline(std::cin, input, '\n');
	processInput(&input);
	glutTimerFunc(60, loopTimer, 0);
}

int main(int argc, char* argv[]){
	//initialize window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(600, 600);
	windowId = glutCreateWindow("L-SYSTEMS");
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{	
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << std::endl;
	initShaders();
	//init buffers and Model View Projection matrix
	genBuffers();	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(300, loopTimer, 0);
	//glutKeyboardFunc(keyPressed);
	glutMainLoop();
	return 0;
}