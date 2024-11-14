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

float bGCr = 0.0, bGCg = 1.0, bGCb = 1.0;
GLuint shaderPID;

static const int index = 24;
static const float polysize = 0.25f;

Shape line;
Shape polygon;
Shape basket;
Shape route;
Shape sliced_polygon[10];
Shape stored_polygon[10];
float basketmove;
int sliced_count;
int stored_count;
int polygontype;
GLenum drawmode = GL_FILL;
bool drag = false;
bool showroute = false;

uniform_int_distribution<int> randtype(3, 4);
uniform_real_distribution<float> randY(0.0f, 0.5f);

void CreateRoute()
{
	glm::vec3 coord[MAX_POINTS];
	float x = polygon.translation.x;
	float y = polygon.translation.y;
	for (int i = 0; i < MAX_POINTS; ++i)
	{
		coord[i] = glm::vec3(x, y, 0);
		x += polygon.speedX;
		y += polygon.speedY + gravity * i;
	}

	route = Shape(MAX_POINTS, coord, glm::vec3(1.0f, 0.0f, 1.0f));
}

void CreatePolygon()
{
	glm::vec3 coord[MAX_POINTS];
	polygontype = randtype(gen);

	switch (polygontype)
	{
	case 3:
		coord[0] = glm::vec3(-polysize / 2.0f, -polysize / 2.0f, 0);
		coord[1] = glm::vec3(0, -polysize / 2.0f, 0);
		coord[2] = glm::vec3(0, polysize / 2.0f, 0);

		coord[3] = glm::vec3(0, -polysize / 2.0f, 0);
		coord[4] = glm::vec3(polysize / 2.0f, -polysize / 2.0f, 0);
		coord[5] = glm::vec3(0, polysize / 2.0f, 0);
		polygon = Shape(6, coord);
		break;
	case 4:
		coord[0] = glm::vec3(-polysize / 2.0f, -polysize / 2.0f, 0);
		coord[1] = glm::vec3(polysize / 2.0f, polysize / 2.0f, 0);
		coord[2] = glm::vec3(-polysize / 2.0f, polysize / 2.0f, 0);

		coord[3] = glm::vec3(-polysize / 2.0f, -polysize / 2.0f, 0);
		coord[4] = glm::vec3(polysize / 2.0f, -polysize / 2.0f, 0);
		coord[5] = glm::vec3(polysize / 2.0f, polysize / 2.0f, 0);
		polygon = Shape(6, coord);
		break;
	default:
		exit(1557);
		break;
	}
	
	if (polygon.speedX >= 0.015f * gamespeed)
	{
		polygon.translation = glm::vec3(-1.0f - polysize, 1.0f - polysize - randY(gen), 0.0f);
	}
	else if (polygon.speedX <= -0.015f * gamespeed)
	{
		polygon.translation = glm::vec3(1.0f + polysize, 1.0f - polysize - randY(gen), 0.0f);
	}
	else
		exit(523);

	CreateRoute();
}

void DeletePolygon(int index)
{
	for (int i = index; i < sliced_count - 1; ++i)
	{
		sliced_polygon[i] = sliced_polygon[i + 1];
	}
	sliced_polygon[--sliced_count] = Shape();
}

void StorePolygon(int index)
{
	if (stored_count >= 10)
	{
		for (int i = 0; i < 9; ++i)
		{
			stored_polygon[i] = stored_polygon[i + 1];
		}
		--stored_count;
	}

	stored_polygon[stored_count++] = sliced_polygon[index];
	DeletePolygon(index);
}

void DivinePolygon()
{
	glm::vec3 coord[3];

	switch (polygontype)
	{
	case 3:
		coord[0] = polygon.shapecoord[0];
		coord[1] = polygon.shapecoord[1];
		coord[2] = polygon.shapecoord[2];
		sliced_polygon[sliced_count] = Shape(3, coord, polygon.shapecolor[0]);
		sliced_polygon[sliced_count].translation = polygon.translation;
		sliced_polygon[sliced_count].speedX = -glm::abs(polygon.speedX);
		sliced_polygon[sliced_count].speedY = 0;
		++sliced_count;

		coord[0] = polygon.shapecoord[3];
		coord[1] = polygon.shapecoord[4];
		coord[2] = polygon.shapecoord[5];
		sliced_polygon[sliced_count] = Shape(3, coord, polygon.shapecolor[0]);
		sliced_polygon[sliced_count].translation = polygon.translation;
		sliced_polygon[sliced_count].speedX = glm::abs(polygon.speedX);
		sliced_polygon[sliced_count].speedY = 0;
		++sliced_count;
		break;
	case 4:
		coord[0] = polygon.shapecoord[0];
		coord[1] = polygon.shapecoord[1];
		coord[2] = polygon.shapecoord[2];
		sliced_polygon[sliced_count] = Shape(3, coord, polygon.shapecolor[0]);
		sliced_polygon[sliced_count].translation = polygon.translation;
		sliced_polygon[sliced_count].speedX = -glm::abs(polygon.speedX);
		sliced_polygon[sliced_count].speedY = 0;
		++sliced_count;

		coord[0] = polygon.shapecoord[3];
		coord[1] = polygon.shapecoord[4];
		coord[2] = polygon.shapecoord[5];
		sliced_polygon[sliced_count] = Shape(3, coord, polygon.shapecolor[0]);
		sliced_polygon[sliced_count].translation = polygon.translation;
		sliced_polygon[sliced_count].speedX = glm::abs(polygon.speedX);
		sliced_polygon[sliced_count].speedY = 0;
		++sliced_count;
		break;
	default:
		break;
	}
}

void InitializeData()
{
	glm::vec3 temp[2] = { glm::vec3(0.0f) };
	line = Shape(2, temp, glm::vec3(0.0f));

	glm::vec3 temp2[4];
	float basketW = 0.2f;
	float basketH = 0.05f;
	temp2[0] = glm::vec3(-basketW, -basketH, 0);
	temp2[1] = glm::vec3(basketW, -basketH, 0);
	temp2[2] = glm::vec3(basketW, basketH, 0);
	temp2[3] = glm::vec3(-basketW, basketH, 0);
	basket = Shape(4, temp2, glm::vec3(1.0f, 0.0f, 0.0f));
	basket.translation = glm::vec3(0.0f, -0.8f, 0.0f);

	CreatePolygon();
	sliced_count = 0;
	stored_count = 0;
	basketmove = 0.01f;
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
	glBindVertexArray(vao);

	UpdateBuffer();

	glPolygonMode(GL_FRONT_AND_BACK, drawmode);

	for (int i = 0; i < sliced_count; ++i)
	{
		sliced_polygon[i].Draw(i);
	}

	for (int i = 0; i < stored_count; ++i)
	{
		stored_polygon[i].Draw(10 + i);
	}

	basket.Draw(22, GL_TRIANGLE_FAN);

	polygon.Draw(20, GL_TRIANGLES);

	if (drag)
		line.Draw(21, GL_LINES);

	if (showroute)
		route.Draw(23, GL_LINE_STRIP);

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
	switch (key)
	{
	case 'l':
		drawmode = GL_LINE;
		break;
	case 'f':
		drawmode = GL_FILL;
		break;
	case 'r':
		showroute = true;
		break;
	case 'R':
		showroute = false;
		break;
	case '+':
		if(gamespeed > 2.0f)
			gamespeed += 0.1f;
		break;
	case '-':
		if(gamespeed > 0.1f)
			gamespeed -= 0.1f;
		break;
	default:
		break;
	}

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

		if ((line.shapecoord[0].x >= line.shapecoord[1].x && polygon.Left() <= line.shapecoord[0].x && polygon.Right() >= line.shapecoord[1].x
				|| line.shapecoord[1].x >= line.shapecoord[0].x && polygon.Left() <= line.shapecoord[1].x && polygon.Right() >= line.shapecoord[0].x)
			&&
			(line.shapecoord[0].y >= line.shapecoord[1].y && polygon.Bottom() <= line.shapecoord[0].y && polygon.Top() >= line.shapecoord[1].y
				|| line.shapecoord[1].y >= line.shapecoord[0].y && polygon.Bottom() <= line.shapecoord[1].y && polygon.Top() >= line.shapecoord[0].y))
		{
			DivinePolygon();
			CreatePolygon();
		}
	}
}

GLvoid Timer(int value)
{
	polygon.MovebyTime();
	if (polygon.Top() < -1.0f || polygon.speedX > 0 && polygon.Left() > 1.0f || polygon.speedX < 0 && polygon.Right() < -1.0f)
		CreatePolygon();

	basket.translation.x += basketmove * gamespeed;
	for (int i = 0; i < stored_count; ++i)
		stored_polygon[i].translation.x += basketmove * gamespeed;
	if (basketmove > 0.0f && basket.Right() >= 1.0f || basketmove < 0.0f && basket.Left() <= -1.0f)
		basketmove *= -1.0f;

	for (int i = 0; i < sliced_count; ++i)
	{
		sliced_polygon[i].MovebyTime();
		if (sliced_polygon[i].Top() < -1.0f || sliced_polygon[i].speedX > 0 && sliced_polygon[i].Left() > 1.0f || sliced_polygon[i].speedX < 0 && sliced_polygon[i].Right() < -1.0f)
			DeletePolygon(i--);
		else if (sliced_polygon[i].Left() <= basket.Right() && sliced_polygon[i].Right() >= basket.Left()
			&& sliced_polygon[i].Bottom() <= basket.Top() && sliced_polygon[i].Top() >= basket.Bottom())
			StorePolygon(i--);
	}

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

	for (int i = 0; i < 10; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, (10 + i) * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(stored_polygon[i].shapecoord[0]));

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, (10 + i) * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(stored_polygon[i].shapecolor[0]));
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 20 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(polygon.shapecoord[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 20 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(polygon.shapecolor[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 21 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(line.shapecoord[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 21 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(line.shapecolor[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 22 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(basket.shapecoord[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 22 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(basket.shapecolor[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 23 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(route.shapecoord[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 23 * MAX_POINTS * 3 * sizeof(GLfloat), MAX_POINTS * 3 * sizeof(GLfloat), glm::value_ptr(route.shapecolor[0]));

}