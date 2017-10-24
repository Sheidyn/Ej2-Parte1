// Tema2-parte1.cpp: define el punto de entrada de la aplicaci�n de consola.
//

#include "stdafx.h"

void draw1Cube();
void initVAO();

void loadSource(GLuint &shaderID, std::string name);
void printCompileInfoLog(GLuint shadID);
void printLinkInfoLog(GLuint programID);
void validateProgram(GLuint programID);

bool init();
void display();
void resize(int, int);
void idle();
void keyboard(unsigned char, int, int);
void specialKeyboard(int, int, int);
void mouse(int, int, int, int);
void mouseMotion(int, int);


bool fullscreen = false;
bool mouseDown = false;
bool animation = true;

float xrot = 0.0f;
float yrot = 0.0f;
float xdiff = 0.0f;
float ydiff = 0.0f;

int g_Width = 500;                          // Ancho inicial de la ventana
int g_Height = 500;                         // Altura incial de la ventana

GLuint vaoHandle;
GLuint vertexShaderObject;
GLuint fragmentShaderObject;
GLuint program;

GLuint modelMatIdx;
GLuint viewMatIdx;
GLuint projMatIdx;

glm::mat4 projMat;
glm::mat4 viewMat;
glm::mat4 modelMat;


// cubo ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// Coordenadas del vertex array  =====================================
// Un cubo tiene 6 lados y cada lado tiene 2 triangles, por tanto, un cubo
// tiene 36 v�rtices (6 lados * 2 trian * 3 vertices = 36 vertices). Y cada
// vertice tiene 4 components (x,y,z) de reales, por tanto, el tama�o del vertex
// array es 144 floats (36 * 4 = 144).
GLfloat vertices1[] = { 1, 1, 1, 1,  -1, 1, 1, 1,  -1,-1, 1, 1,     // v0-v1-v2 (front)
-1,-1, 1, 1,   1,-1, 1, 1,   1, 1, 1, 1,     // v2-v3-v0

1, 1, 1, 1,   1,-1, 1, 1,   1,-1,-1, 1,      // v0-v3-v4 (right)
1,-1,-1, 1,   1, 1,-1, 1,   1, 1, 1, 1,      // v4-v5-v0

1, 1, 1, 1,   1, 1,-1, 1,  -1, 1,-1, 1,      // v0-v5-v6 (top)
-1, 1,-1, 1,  -1, 1, 1, 1,   1, 1, 1, 1,      // v6-v1-v0

-1, 1, 1, 1,  -1, 1,-1, 1,  -1,-1,-1, 1,      // v1-v6-v7 (left)
-1,-1,-1, 1,  -1,-1, 1, 1,  -1, 1, 1, 1,      // v7-v2-v1

-1,-1,-1, 1,   1,-1,-1, 1,   1,-1, 1, 1,      // v7-v4-v3 (bottom)
1,-1, 1, 1,  -1,-1, 1, 1,  -1,-1,-1, 1,      // v3-v2-v7

1,-1,-1, 1,  -1,-1,-1, 1,  -1, 1,-1, 1,      // v4-v7-v6 (back)
-1, 1,-1, 1,   1, 1,-1, 1,   1,-1,-1, 1 };    // v6-v5-v4
											  // color array
GLfloat colores1[] = { 1, 1, 1, 1,   1, 1, 0, 1,   1, 0, 0, 1,      // v0-v1-v2 (front)
1, 0, 0, 1,   1, 0, 1, 1,   1, 1, 1, 1,      // v2-v3-v0

1, 1, 1, 1,   1, 0, 1, 1,   0, 0, 1, 1,      // v0-v3-v4 (right)
0, 0, 1, 1,   0, 1, 1, 1,   1, 1, 1, 1,      // v4-v5-v0

1, 1, 1, 1,   0, 1, 1, 1,   0, 1, 0, 1,      // v0-v5-v6 (top)
0, 1, 0, 1,   1, 1, 0, 1,   1, 1, 1, 1,      // v6-v1-v0

1, 1, 0, 1,   0, 1, 0, 1,   0, 0, 0, 1,      // v1-v6-v7 (left)
0, 0, 0, 1,   1, 0, 0, 1,   1, 1, 0, 1,      // v7-v2-v1

0, 0, 0, 1,   0, 0, 1, 1,   1, 0, 1, 1,      // v7-v4-v3 (bottom)
1, 0, 1, 1,   1, 0, 0, 1,   0, 0, 0, 1,      // v3-v2-v7

0, 0, 1, 1,   0, 0, 0, 1,   0, 1, 0, 1,      // v4-v7-v6 (back)
0, 1, 0, 1,   0, 1, 1, 1,   0, 0, 1, 1 };    // v6-v5-v4



											 // BEGIN: Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////

void loadSource(GLuint &shaderID, std::string name)
{
	std::ifstream f(name.c_str());
	if (!f.is_open())
	{
		std::cerr << "File not found " << name.c_str() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	// now read in the data
	std::string *source;
	source = new std::string(std::istreambuf_iterator<char>(f),
		std::istreambuf_iterator<char>());
	f.close();

	// add a null to the string
	*source += "\0";
	const GLchar * data = source->c_str();
	glShaderSource(shaderID, 1, &data, NULL);
	delete source;
}

void printCompileInfoLog(GLuint shadID)
{
	GLint compiled;
	glGetShaderiv(shadID, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetShaderiv(shadID, GL_INFO_LOG_LENGTH, &infoLength);

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetShaderInfoLog(shadID, infoLength, &chsWritten, infoLog);

		std::cerr << "Shader compiling failed:" << infoLog << std::endl;
		system("pause");
		delete[] infoLog;

		exit(EXIT_FAILURE);
	}
}

void printLinkInfoLog(GLuint programID)
{
	GLint linked;
	glGetProgramiv(programID, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLength);

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetProgramInfoLog(programID, infoLength, &chsWritten, infoLog);

		std::cerr << "Shader linking failed:" << infoLog << std::endl;
		system("pause");
		delete[] infoLog;

		exit(EXIT_FAILURE);
	}
}

void validateProgram(GLuint programID)
{
	GLint status;
	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);

	if (status == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLength);

		if (infoLength > 0)
		{
			GLchar *infoLog = new GLchar[infoLength];
			GLint chsWritten = 0;
			glGetProgramInfoLog(programID, infoLength, &chsWritten, infoLog);
			std::cerr << "Program validating failed:" << infoLog << std::endl;
			system("pause");
			delete[] infoLog;

			exit(EXIT_FAILURE);
		}
	}
}

// END:   Soporte shaders //////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Init para draw1Cube
///////////////////////////////////////////////////////////////////////////////
void initVAO()
{
	GLuint vboHandle;

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	/*glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);*/

	glGenBuffers(1, &vboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(colores1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(colores1), colores1);

	/*glVertexPointer(4, GL_FLOAT, 0, (char*)NULL + 0);
	glColorPointer(4, GL_FLOAT, 0, (char*)NULL + sizeof(GL_FLOAT) * 4 * 36);*/
	GLuint loc = glGetAttribLocation(program, "aPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	GLuint loc2 = glGetAttribLocation(program, "aColor");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer( loc2, 4, GL_FLOAT, GL_FALSE, 0,(char *)NULL + sizeof(vertices1));
	// GLuint loc3 = glGetUniformLocation(program, "uModelViewProj");

	modelMatIdx = glGetUniformLocation(program, "modelMat");
	viewMatIdx = glGetUniformLocation(program, "viewMat");
	projMatIdx = glGetUniformLocation(program, "projMat");

	glBindVertexArray(0);
}


///////////////////////////////////////////////////////////////////////////////
// Dibuja cubos utilizando VAO no entrelazados y no indexados 
///////////////////////////////////////////////////////////////////////////////
void draw1Cube()
{
	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		system("pause");
		exit(-1);
	}
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();

	return EXIT_SUCCESS;
}

bool init()
{
	
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	// Tarea por hacer (ejer. 1): Aqu� creamos el objeto programa.
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	loadSource(vertexShaderObject, "tema2_ej1.vert");
	loadSource(fragmentShaderObject, "tema2_ej1.frag");

	glCompileShader(vertexShaderObject);
	printf("vertexShaderObject \n");
	printCompileInfoLog(vertexShaderObject);

	glCompileShader(fragmentShaderObject);
	printf("fragmentShaderObject \n");
	printCompileInfoLog(fragmentShaderObject);

	program = glCreateProgram();

	glAttachShader(program, fragmentShaderObject);
	glAttachShader(program, vertexShaderObject);
	
	glLinkProgram(program);
	printLinkInfoLog(program);

	validateProgram(program);

	initVAO();

	return true;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		0.0f, 0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	// Tarea por hacer (ejer. 3): Sustituir el anterior c�digo por el c�digo siguiente.
	/*
	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, yrot, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 mvp = Projection * View * Model;
	*/
	// Tarea por hacer (ejer. 1): Aqu� activamos el objeto programa.
	projMat = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	viewMat = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	modelMat = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f, 1.0f, 1.0f));
	//modelMat = glm::rotate(modelMat, xrot, glm::vec3(0.0f, 1.0f, 0.0f));
	glUseProgram(program);

	glUniformMatrix4fv(projMatIdx, 1, GL_FALSE, glm::value_ptr(projMat));
	glUniformMatrix4fv(viewMatIdx, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(modelMatIdx, 1, GL_FALSE, glm::value_ptr(modelMat));
	
	draw1Cube();

	// Tarea por hacer (ejer. 1): Aqu� lo desactivamos.

	glUseProgram(0);
	glutSwapBuffers();
}

void resize(int w, int h)
{
	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
}

void idle()
{
	if (!mouseDown && animation)
	{
		xrot += 0.3f;
		yrot += 0.4f;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: case 'q': case 'Q':
		// Tarea por hacer (ejer. 1): Aqu� borramos el objeto programa.
		exit(1);
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	}
}

void specialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;

		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(g_Width, g_Height);
			glutPositionWindow(50, 50);
		}
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;

		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
}

void mouseMotion(int x, int y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;

		glutPostRedisplay();
	}
}

