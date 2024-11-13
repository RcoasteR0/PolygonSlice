#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <random>
#include <cmath>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Shape.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60

using namespace std;

GLuint vao, vbo[2], ebo;
void convertXY(int x, int y, float& fx, float& fy);
void UpdateBuffer();
void InitBuffer();
void InitializeData();
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Timer(int value);

float bGCr = 1.0, bGCg = 1.0, bGCb = 1.0;
GLuint shaderPID;

static const int index = 12;

Shape line;
Shape polygon;
Shape sliced_polygon[10];
bool drag = false;

uniform_int_distribution<int> randpoly(3, 4);

void CreatePolygon()
{
	switch (randpoly(gen))
	{
	case 3:
		break;
	default:
		break;
	}
}

void InitializeData()
{
	glm::vec3 temp[2] = { glm::vec3(0.0f) };
	line = Shape(2, temp, glm::vec3(0.0f));
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	InitializeData();

	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutReshapeFunc(Reshape);
	glutTimerFunc(1000 / FPS, Timer, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bGCr, bGCg, bGCb, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	UpdateBuffer();
	glBindVertexArray(vao);

	if(drag)
		line.Draw(11, GL_LINES);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void convertXY(int x, int y, float& fx, float& fy)
{
	int w = WINDOW_WIDTH;
	int h = WINDOW_HEIGHT;
	fx = (float)((x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0)));
	fy = -(float)((y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0)));
}

GLvoid Keyboard(unsigned char key, int x, int y)
{


	if(key == 'q')
		glutLeaveMainLoop();

	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	float fx = 0.0, fy = 0.0;
	convertXY(x, y, fx, fy);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		drag = true;

		line.shapecoord[0] = glm::vec3(fx, fy, 0);
		line.shapecoord[1] = glm::vec3(fx, fy, 0);
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		drag = false;
	}
}

GLvoid Timer(int value)
{

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, Timer, 1);
}

void Motion(int x, int y)
{
	float fx = 0.0, fy = 0.0;
	convertXY(x, y, fx, fy);

	if (drag)
	{
		line.shapecoord[1] = glm::vec3(fx, fy, 0);
	}

	glutPostRedisplay();
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기


	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, index * MAX_POINTS * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float

	glBufferData(GL_ARRAY_BUFFER, index * MAX_POINTS * 3 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

}

void UpdateBuffer()
{
	for (int i = 0; i < 10; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, i * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(sliced_polygon[i].shapecoord[0]));

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, i * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(sliced_polygon[i].shapecolor[0]));
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 10 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(polygon.shapecoord[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 10 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(polygon.shapecolor[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 11 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(line.shapecoord[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 11 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(line.shapecolor[0]));

}