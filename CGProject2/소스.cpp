#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <random>
#include <cmath>
#include "Shader.h"

#define Quiz12
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60

using namespace std;

random_device rd;
mt19937 gen(rd());

GLuint vao, vbo[2], ebo;
void convertXY(int x, int y, float& fx, float& fy);
void UpdateBuffer();
void InitBuffer();
void Initialize();
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Timer(int value);

float bGCr = 1.0, bGCg = 1.0, bGCb = 1.0;
GLuint shaderPID;

struct Old_Shape
{
	GLfloat shapecoord[4][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };
	GLfloat shapecolor[4][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };
	int state = 0;
	bool states = false;
};

class Shape
{
public:
	GLfloat shapecoord[6][3] = {};
	GLfloat shapecolor[6][3] = {};
	float left, bottom, right, top;
	int points;

	Shape()
	{
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				shapecoord[i][j] = 0.0f;
				shapecolor[i][j] = 0.0f;
			}
		}
		left = 0.0f, bottom = 0.0f, right = 0.0f, top = 0.0f;
		points = 0;
	}

	Shape(int state, GLfloat coord[][3], GLfloat color[3])
	{
		left = FLT_MAX, bottom = FLT_MAX;
		right = FLT_MIN, top = FLT_MIN;
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (i >= state)
				{
					shapecoord[i][j] = 0.0f;
					shapecolor[i][j] = 0.0f;
				}
				else
				{
					shapecoord[i][j] = coord[i][j];
					shapecolor[i][j] = color[j];
				}
			}

			if (shapecoord[i][0] <= left && i < state)
				left = shapecoord[i][0];
			if (shapecoord[i][1] <= bottom && i < state)
				bottom = shapecoord[i][1];
			if (shapecoord[i][0] >= right && i < state)
				right = shapecoord[i][0];
			if (shapecoord[i][1] >= top && i < state)
				top = shapecoord[i][1];

		}
		points = state;
	}

	Shape(int state, float x, float y, GLfloat color[3], float size = 0.1f)
	{
		switch (state)
		{
		case 1:
			shapecoord[0][0] = x;
			shapecoord[0][1] = y;
			break;
		case 2:
			shapecoord[0][0] = x - size;
			shapecoord[0][1] = y - size;

			shapecoord[1][0] = x + size;
			shapecoord[1][1] = y + size;
			break;
		case 3:
			shapecoord[0][0] = x - size;
			shapecoord[0][1] = y - size;

			shapecoord[1][0] = x + size;
			shapecoord[1][1] = y - size;

			shapecoord[2][0] = x;
			shapecoord[2][1] = y + size;
			break;
		case 4:
			shapecoord[0][0] = x - size;
			shapecoord[0][1] = y - size;

			shapecoord[1][0] = x + size;
			shapecoord[1][1] = y - size;

			shapecoord[2][0] = x + size;
			shapecoord[2][1] = y + size;

			shapecoord[3][0] = x - size;
			shapecoord[3][1] = y + size;
			break;
		case 5:
			shapecoord[0][0] = x - size / 2;
			shapecoord[0][1] = y - size;

			shapecoord[1][0] = x + size / 2;
			shapecoord[1][1] = y - size;

			shapecoord[2][0] = x + size;
			shapecoord[2][1] = y;

			shapecoord[3][0] = x;
			shapecoord[3][1] = y + size;

			shapecoord[4][0] = x - size;
			shapecoord[4][1] = y;
			break;
		case 6:
			shapecoord[0][0] = x - size / 2;
			shapecoord[0][1] = y - size;

			shapecoord[1][0] = x + size / 2;
			shapecoord[1][1] = y - size;

			shapecoord[2][0] = x + size;
			shapecoord[2][1] = y;

			shapecoord[3][0] = x + size / 2;
			shapecoord[3][1] = y + size;

			shapecoord[4][0] = x - size / 2;
			shapecoord[4][1] = y + size;

			shapecoord[5][0] = x - size;
			shapecoord[5][1] = y;
			break;
		default:
			break;
		}

		for (int i = 0; i < 6; ++i)
			for (int j = 0; j < 3; ++j)
				shapecolor[i][j] = color[j];

		if (state == 1)
		{
			left = x - 0.01;
			bottom = y - 0.01;
			right = x + 0.01;
			top = y + 0.01;
		}
		else
		{
			left = x - size;
			bottom = y - size;
			right = x + size;
			top = y + size;
		}

		points = state;
	}

	~Shape() {}

	void Draw(int i)
	{
		if (points == 1)
		{
			glPointSize(5.0);
			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(GLuint)));
		}
		else if (points == 2)
		{
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(GLuint)));
		}
		else
			glDrawElements(GL_TRIANGLE_FAN, points, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(GLuint)));
	}
};

void RandomColor(float& colorR, float& colorG, float& colorB)
{
	uniform_real_distribution<float> random(0, 1);
	colorR = random(gen);
	colorG = random(gen);
	colorB = random(gen);
}

#ifdef Quiz7
int randomIndex()
{
	uniform_int_distribution<int> random(0, 9);
	return random(gen);
}

Old_Shape shape[10];
const int SHAPES = 10;
int shapecount;

void Initialize()
{
	shapecount = 0;
}
#endif // Quiz7

#ifdef Quiz8
uniform_real_distribution<GLfloat> randsize(0.05, 0.3);
const int SHAPES = 3;
Old_Shape triangles[4][3];
int shapecount[4] = {};
bool filltri;

void Initialize()
{
	filltri = true;
}
#endif // Quiz8

#ifdef Quiz9
enum Move { STOP, DIAGONAL, ZIGZAG, RECTSPIRAL, CIRCLESPIRAL };

uniform_real_distribution<GLfloat> randsize(0.05, 0.3);
Old_Shape triangles[4];
Old_Shape origtriangles[4];
Move movetype = STOP;
float moveX[4] = {};
float moveY[4] = {};
float limitX[2] = {};
float limitY[2] = {};
float radius = 0.0f;
int angle = 0;

void Initialize()
{
	triangles[0].shapecoord[0][0] = 0.5;
	triangles[0].shapecoord[0][1] = 0.5 + sqrt(2) * 0.1;
	triangles[0].shapecoord[1][0] = 0.5 - 0.1;
	triangles[0].shapecoord[1][1] = 0.5 - 0.1;
	triangles[0].shapecoord[2][0] = 0.5 + 0.1;
	triangles[0].shapecoord[2][1] = 0.5 - 0.1;

	triangles[1].shapecoord[0][0] = -0.5;
	triangles[1].shapecoord[0][1] = 0.5 + sqrt(2) * 0.1;
	triangles[1].shapecoord[1][0] = -0.5 - 0.1;
	triangles[1].shapecoord[1][1] = 0.5 - 0.1;
	triangles[1].shapecoord[2][0] = -0.5 + 0.1;
	triangles[1].shapecoord[2][1] = 0.5 - 0.1;

	triangles[2].shapecoord[0][0] = -0.5;
	triangles[2].shapecoord[0][1] = -0.5 + sqrt(2) * 0.1;
	triangles[2].shapecoord[1][0] = -0.5 - 0.1;
	triangles[2].shapecoord[1][1] = -0.5 - 0.1;
	triangles[2].shapecoord[2][0] = -0.5 + 0.1;
	triangles[2].shapecoord[2][1] = -0.5 - 0.1;

	triangles[3].shapecoord[0][0] = 0.5;
	triangles[3].shapecoord[0][1] = -0.5 + sqrt(2) * 0.1;
	triangles[3].shapecoord[1][0] = 0.5 - 0.1;
	triangles[3].shapecoord[1][1] = -0.5 - 0.1;
	triangles[3].shapecoord[2][0] = 0.5 + 0.1;
	triangles[3].shapecoord[2][1] = -0.5 - 0.1;

	for (int i = 0; i < 4; ++i)
	{
		GLfloat randR, randG, randB;
		RandomColor(randR, randG, randB);

		for (int j = 0; j < 3; ++j)
		{
			triangles[i].shapecolor[j][0] = randR;
			triangles[i].shapecolor[j][1] = randG;
			triangles[i].shapecolor[j][2] = randB;
		}

		origtriangles[i] = triangles[i];
	}

	movetype = STOP;
}

void MoveOrigPos()
{
	for (int i = 0; i < 4; ++i)
	{
		triangles[i] = origtriangles[i];
	}
}

bool CheckColideWall_Left(Old_Shape tri, float wall = -1.0f)
{
	return tri.shapecoord[1][0] <= wall;
}
bool CheckColideWall_Right(Old_Shape tri, float wall = 1.0f)
{
	return tri.shapecoord[2][0] >= wall;
}

bool CheckColideWall_Top(Old_Shape tri, float wall = 1.0f)
{
	return tri.shapecoord[0][1] >= wall;
}
bool CheckColideWall_Bottom(Old_Shape tri, float wall = -1.0f)
{
	return tri.shapecoord[1][1] <= wall;
}
#endif // Quiz8

#ifdef Quiz10
const int pointcount = 180;
GLfloat points[5][pointcount][3];
float radius;
int angle;
int spiralcount;
int progress;
bool draw;
bool line;

void Initialize()
{
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < pointcount; ++j)
		{
			points[i][j][0] = 0.0f;
			points[i][j][1] = 0.0f;
			points[i][j][2] = 0.0f;
		}
	}
	radius = 0.0f;
	angle = 0;
	spiralcount = 1;
	progress = 0;
	draw = false;
	line = false;
	bGCr = 1.0, bGCg = 1.0, bGCb = 1.0;
}
#endif // Quiz10

#ifdef Quiz11
enum Drawtype{ ALL, LINE, TRIANGLE, RECTANGLE, PENTAGON };
Shape shapes[4];
Drawtype type;
float timer;

void Initialize()
{
	GLfloat temp1[6][3] = { { -0.75f, 0.25f, 0.0f }, { -0.749f, 0.25f, 0.0f }, { -0.25f, 0.74f, 0.0f }, { -0.25f, 0.75f, 0.0f } };
	GLfloat temp2[6][3] = { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.75f, 0.75f, 0.0f }, { 0.75f, 0.75f, 0.0f } };
	GLfloat temp3[6][3] = { { -0.75f, -0.75f, 0.0f }, { -0.25f, -0.75f, 0.0f }, { -0.25f, -0.5f, 0.0f }, { -0.5f, -0.5f, 0.0f }, { -0.75f, -0.5f, 0.0f } };
	GLfloat temp4[6][3] = { { 0.4f, -0.75f, 0.0f }, { 0.6f, -0.75f, 0.0f }, { 0.75f, -0.5f, 0.0f }, { 0.5f, -0.25f, 0.0f }, { 0.25f, -0.5f, 0.0f } };
	GLfloat red[3] = { 1.0f, 0.0f, 0.0f };
	GLfloat blue[3] = { 0.0f, 0.0f, 1.0f };
	GLfloat yellow[3] = { 1.0f, 1.0f, 0.0f };
	GLfloat magenta[3] = { 1.0f, 0.0f, 1.0f };

	shapes[0] = Shape(4, temp1, red);
	shapes[1] = Shape(4, temp2, blue);
	shapes[2] = Shape(5, temp3, yellow);
	shapes[3] = Shape(5, temp4, magenta);

	type = ALL;
	timer = 0.0f;
}
#endif // Quiz11

#ifdef Quiz12
Shape shapes[15];
Shape mergedshapes[8];
float prevX = 0.0f, prevY = 0.0f;
int mergecount;
int dragshape;
bool drag;
bool merged[15];

uniform_real_distribution<float> randomcoord(-0.9f, 0.9f);

void Initialize()
{
	GLfloat color[3] = {};
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			RandomColor(color[0], color[1], color[2]);
			shapes[i * 3 + j] = Shape(i + 1, randomcoord(gen), randomcoord(gen), color);
			merged[i * 3 + j] = false;
		}
	}
	mergecount = 0;
	dragshape = 0;
	drag = false;
}
#endif // Quiz12

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

#ifdef Quiz8
	triangles[0][0].shapecoord[0][0] = 0.5;
	triangles[0][0].shapecoord[0][1] = 0.5 + sqrt(2) * 0.1;
	triangles[0][0].shapecoord[1][0] = 0.5 - 0.1;
	triangles[0][0].shapecoord[1][1] = 0.5 - 0.1;
	triangles[0][0].shapecoord[2][0] = 0.5 + 0.1;
	triangles[0][0].shapecoord[2][1] = 0.5 - 0.1;

	triangles[1][0].shapecoord[0][0] = -0.5;
	triangles[1][0].shapecoord[0][1] = 0.5 + sqrt(2) * 0.1;
	triangles[1][0].shapecoord[1][0] = -0.5 - 0.1;
	triangles[1][0].shapecoord[1][1] = 0.5 - 0.1;
	triangles[1][0].shapecoord[2][0] = -0.5 + 0.1;
	triangles[1][0].shapecoord[2][1] = 0.5 - 0.1;

	triangles[2][0].shapecoord[0][0] = -0.5;
	triangles[2][0].shapecoord[0][1] = -0.5 + sqrt(2) * 0.1;
	triangles[2][0].shapecoord[1][0] = -0.5 - 0.1;
	triangles[2][0].shapecoord[1][1] = -0.5 - 0.1;
	triangles[2][0].shapecoord[2][0] = -0.5 + 0.1;
	triangles[2][0].shapecoord[2][1] = -0.5 - 0.1;

	triangles[3][0].shapecoord[0][0] = 0.5;
	triangles[3][0].shapecoord[0][1] = -0.5 + sqrt(2) * 0.1;
	triangles[3][0].shapecoord[1][0] = 0.5 - 0.1;
	triangles[3][0].shapecoord[1][1] = -0.5 - 0.1;
	triangles[3][0].shapecoord[2][0] = 0.5 + 0.1;
	triangles[3][0].shapecoord[2][1] = -0.5 - 0.1;

	for (int i = 0; i < 4; ++i)
	{
		GLfloat randR, randG, randB;
		RandomColor(randR, randG, randB);

		for (int j = 0; j < 3; ++j)
		{
			triangles[i][0].shapecolor[j][0] = randR;
			triangles[i][0].shapecolor[j][1] = randG;
			triangles[i][0].shapecolor[j][2] = randB;
		}

		triangles[i][0].state = 3;
		triangles[i][0].states = true;
		shapecount[i] = 1;
	}
#endif // Quiz8
	Initialize();

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
#ifdef Quiz7
	for (int i = 0; i < 10; i++)
	{
		if (shape[i].states)
		{
			if (shape[i].state == 1)
			{
				glPointSize(5.0);
				glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(i * 4 * sizeof(GLuint)));
			}
			else if (shape[i].state == 2)
			{
				glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * 4 * sizeof(GLuint)));
			}
			else if (shape[i].state == 3)
			{
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 4 * sizeof(GLuint)));
			}
			else if (shape[i].state == 4)
			{
				glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)(i * 4 * sizeof(GLuint)));
			}
		}
	}
#endif // Quiz7
#ifdef Quiz8
	if (filltri)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < shapecount[i]; ++j)
		{
			if (triangles[i][j].states)
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)((i * SHAPES + j) * 4 * sizeof(GLuint)));
		}
	}

#endif // Quiz8
#ifdef Quiz9
	for (int i = 0; i < 4; i++)
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 4 * sizeof(GLuint)));
	}
#endif // Quiz9
#ifdef Quiz10
	glPointSize(3.0f);
	if (line)
	{
		for (int i = 0; i < spiralcount; i++)
		{
			glDrawElements(GL_LINE_STRIP, progress, GL_UNSIGNED_INT, (void*)(i * pointcount * sizeof(GLuint)));
		}
	}
	else
	{
		for (int i = 0; i < spiralcount; i++)
		{
			glDrawElements(GL_POINTS, progress, GL_UNSIGNED_INT, (void*)(i * pointcount * sizeof(GLuint)));
		}
	}
#endif // Quiz10
#ifdef Quiz11
	switch (type)
	{
	case ALL:
		for (int i = 0; i < 4; i++)
		{
			shapes[i].Draw(i);
		}
		break;
	case LINE:
		shapes[0].Draw(0);
		break;
	case TRIANGLE:
		shapes[1].Draw(1);
		break;
	case RECTANGLE:
		shapes[2].Draw(2);
		break;
	case PENTAGON:
		shapes[3].Draw(3);
		break;
	default:
		break;
	}
#endif // Quiz11
#ifdef Quiz12
	for (int i = 0; i < 15; ++i)
		shapes[i].Draw(i);
#endif // Quiz12

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
#ifdef Quiz7
	int a = randomIndex();
	switch (key)
	{
	case 'p':
		shape[shapecount].state = 1;
		break;
	case 'l':
		shape[shapecount].state = 2;
		break;
	case 't':
		shape[shapecount].state = 3;
		break;
	case 'r':
		shape[shapecount].state = 4;
		break;
	case 'w':
		switch (shape[a].state)
		{
		case 1:
			shape[a].shapecoord[0][1] += 0.05;
			break;
		case 2:
			shape[a].shapecoord[0][1] += 0.05;
			shape[a].shapecoord[1][1] += 0.05;
			break;
		case 3:
			shape[a].shapecoord[0][1] += 0.05;
			shape[a].shapecoord[1][1] += 0.05;
			shape[a].shapecoord[2][1] += 0.05;
			break;
		case 4:
			shape[a].shapecoord[0][1] += 0.05;
			shape[a].shapecoord[1][1] += 0.05;
			shape[a].shapecoord[2][1] += 0.05;
			shape[a].shapecoord[3][1] += 0.05;
			break;
		}
		break;
	case 'a':
		switch (shape[a].state)
		{
		case 1:
			shape[a].shapecoord[0][0] -= 0.05;
			break;
		case 2:
			shape[a].shapecoord[0][0] -= 0.05;
			shape[a].shapecoord[1][0] -= 0.05;
			break;
		case 3:
			shape[a].shapecoord[0][0] -= 0.05;
			shape[a].shapecoord[1][0] -= 0.05;
			shape[a].shapecoord[2][0] -= 0.05;
			break;
		case 4:
			shape[a].shapecoord[0][0] -= 0.05;
			shape[a].shapecoord[1][0] -= 0.05;
			shape[a].shapecoord[2][0] -= 0.05;
			shape[a].shapecoord[3][0] -= 0.05;
			break;
		}
		break;
	case 's':
		switch (shape[a].state)
		{
		case 1:
			shape[a].shapecoord[0][1] -= 0.05;
			break;
		case 2:
			shape[a].shapecoord[0][1] -= 0.05;
			shape[a].shapecoord[1][1] -= 0.05;
			break;
		case 3:
			shape[a].shapecoord[0][1] -= 0.05;
			shape[a].shapecoord[1][1] -= 0.05;
			shape[a].shapecoord[2][1] -= 0.05;
			break;
		case 4:
			shape[a].shapecoord[0][1] -= 0.05;
			shape[a].shapecoord[1][1] -= 0.05;
			shape[a].shapecoord[2][1] -= 0.05;
			shape[a].shapecoord[3][1] -= 0.05;
			break;
		}
		break;
	case 'd':
		switch (shape[a].state)
		{
		case 1:
			shape[a].shapecoord[0][0] += 0.05;
			break;
		case 2:
			shape[a].shapecoord[0][0] += 0.05;
			shape[a].shapecoord[1][0] += 0.05;
			break;
		case 3:
			shape[a].shapecoord[0][0] += 0.05;
			shape[a].shapecoord[1][0] += 0.05;
			shape[a].shapecoord[2][0] += 0.05;
			break;
		case 4:
			shape[a].shapecoord[0][0] += 0.05;
			shape[a].shapecoord[1][0] += 0.05;
			shape[a].shapecoord[2][0] += 0.05;
			shape[a].shapecoord[3][0] += 0.05;
			break;
		}
		break;
	case 'c':
		shapecount = 0;
		for (int i = 0; i < 10; i++)
		{
			shape[i].state = 0;
			shape[i].states = false;
		}
		break;
	}
#endif // Quiz7
#ifdef Quiz8
	switch (key)
	{
	case 'a':
		filltri = true;
		break;
	case 'b':
		filltri = false;
		break;
	default:
		break;
	}
#endif // Quiz8
#ifdef Quiz9
	switch (key)
	{
	case '1':
		movetype = DIAGONAL;
		MoveOrigPos();
		for (int i = 0; i < 4; ++i)
		{
			moveX[i] = 0.05f;
			moveY[i] = 0.05f;
		}
		break;
	case '2':
		movetype = ZIGZAG;
		MoveOrigPos();
		for (int i = 0; i < 4; ++i)
		{
			moveX[i] = 0.05f;
			moveY[i] = 0.3f;
		}

		break;
	case '3':
		movetype = RECTSPIRAL;
		MoveOrigPos();
		for (int i = 0; i < 4; ++i)
		{
			moveX[i] = 0.05f;
			moveY[i] = 0.0f;
		}
		limitX[0] = 0.0f;
		limitX[1] = 0.0f;
		limitY[0] = 0.0f;
		limitY[1] = 0.0f;

		break;
	case '4':
		movetype = CIRCLESPIRAL;
		MoveOrigPos();
		radius = 0.0f;
		angle = 0;
		break;
	case 'r':
		Initialize();
		break;
	default:
		break;
	}
#endif // Quiz9
#ifdef Quiz10
	switch (key)
	{
	case 'p':
		line = false;
		break;
	case 'l':
		line = true;
		break;
	case '1':
		Initialize();
		spiralcount = 1;
		break;
	case '2':
		Initialize();
		spiralcount = 2;
		break;
	case '3':
		Initialize();
		spiralcount = 3;
		break;
	case '4':
		Initialize();
		spiralcount = 4;
		break;
	case '5':
		Initialize();
		spiralcount = 5;
		break;
	default:
		break;
	}
#endif // Quiz10
#ifdef Quiz11
	switch (key)
	{
	case 'l':
		type = LINE;
		break;
	case 't':
		type = TRIANGLE;
		break;
	case 'r':
		type = RECTANGLE;
		break;
	case 'p':
		type = PENTAGON;
		break;
	case 'a':
		type = ALL;
		break;
	case 'c':
		Initialize();
		break;
	default:
		break;
	}
#endif // Quiz11
#ifdef Quiz12
	if (key == 'r')
		Initialize();
#endif // Quiz12

	if(key == 'q')
		glutLeaveMainLoop();

	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	float fx = 0.0, fy = 0.0;
	convertXY(x, y, fx, fy);

#ifdef Quiz7
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && shape[shapecount].state)
	{
		if (shapecount < 10)
		{
			GLfloat randR, randG, randB;
			RandomColor(randR, randG, randB);

			switch (shape[shapecount].state)
			{
			case 1:
				shape[shapecount].shapecoord[0][0] = fx;
				shape[shapecount].shapecoord[0][1] = fy;
				break;
			case 2:
				shape[shapecount].shapecoord[0][0] = fx - 0.25;
				shape[shapecount].shapecoord[0][1] = fy - 0.25;
				shape[shapecount].shapecoord[1][0] = fx + 0.25;
				shape[shapecount].shapecoord[1][1] = fy + 0.25;
				break;
			case 3:
				shape[shapecount].shapecoord[0][0] = fx;
				shape[shapecount].shapecoord[0][1] = fy;
				shape[shapecount].shapecoord[1][0] = fx - 0.1;
				shape[shapecount].shapecoord[1][1] = fy - 0.3;
				shape[shapecount].shapecoord[2][0] = fx + 0.1;
				shape[shapecount].shapecoord[2][1] = fy - 0.3;
				break;
			case 4:
				shape[shapecount].shapecoord[0][0] = fx - 0.25;
				shape[shapecount].shapecoord[0][1] = fy - 0.25;
				shape[shapecount].shapecoord[1][0] = fx + 0.25;
				shape[shapecount].shapecoord[1][1] = fy - 0.25;
				shape[shapecount].shapecoord[2][0] = fx - 0.25;
				shape[shapecount].shapecoord[2][1] = fy + 0.25;
				shape[shapecount].shapecoord[3][0] = fx + 0.25;
				shape[shapecount].shapecoord[3][1] = fy + 0.25;
				break;
			}
			for (int i = 0; i < shape[shapecount].state; ++i)
			{
				shape[shapecount].shapecolor[i][0] = randR;
				shape[shapecount].shapecolor[i][1] = randG;
				shape[shapecount].shapecolor[i][2] = randB;
			}
			shape[shapecount].states = true;
			shapecount++;
			shape[shapecount].state = shape[shapecount - 1].state;
		}
	}
#endif // Quiz7
#ifdef Quiz8
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		GLfloat size = randsize(gen);
		int index = 0;
		if (fx >= 0 && fy >= 0)
		{
			index = 0;
		}
		else if (fx < 0 && fy >= 0)
		{
			index = 1;
		}
		else if (fx < 0 && fy < 0)
		{
			index = 2;
		}
		else if (fx >= 0 && fy < 0)
		{
			index = 3;
		}

		triangles[index][0].shapecoord[0][0] = fx;
		triangles[index][0].shapecoord[0][1] = fy + sqrt(2) * size;
		triangles[index][0].shapecoord[1][0] = fx - size;
		triangles[index][0].shapecoord[1][1] = fy - size;
		triangles[index][0].shapecoord[2][0] = fx + size;
		triangles[index][0].shapecoord[2][1] = fy - size;

		GLfloat randR, randG, randB;
		RandomColor(randR, randG, randB);

		for (int j = 0; j < 3; ++j)
		{
			triangles[index][0].shapecolor[j][0] = randR;
			triangles[index][0].shapecolor[j][1] = randG;
			triangles[index][0].shapecolor[j][2] = randB;
		}

		triangles[index][0].state = 3;
		triangles[index][0].states = true;
		shapecount[index] = 1;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		GLfloat size = randsize(gen);
		int index = 0;
		if (fx >= 0 && fy >= 0)
		{
			index = 0;
		}
		else if (fx < 0 && fy >= 0)
		{
			index = 1;
		}
		else if (fx < 0 && fy < 0)
		{
			index = 2;
		}
		else if (fx >= 0 && fy < 0)
		{
			index = 3;
		}

		if (shapecount[index] >= 3)
		{
			for (int i = 0; i < 2; ++i)
				triangles[index][i] = triangles[index][i + 1];
			--shapecount[index];
		}

		triangles[index][shapecount[index]].shapecoord[0][0] = fx;
		triangles[index][shapecount[index]].shapecoord[0][1] = fy + sqrt(2) * size;
		triangles[index][shapecount[index]].shapecoord[1][0] = fx - size;
		triangles[index][shapecount[index]].shapecoord[1][1] = fy - size;
		triangles[index][shapecount[index]].shapecoord[2][0] = fx + size;
		triangles[index][shapecount[index]].shapecoord[2][1] = fy - size;

		GLfloat randR, randG, randB;
		RandomColor(randR, randG, randB);

		for (int j = 0; j < 3; ++j)
		{
			triangles[index][shapecount[index]].shapecolor[j][0] = randR;
			triangles[index][shapecount[index]].shapecolor[j][1] = randG;
			triangles[index][shapecount[index]].shapecolor[j][2] = randB;
		}

		triangles[index][shapecount[index]].state = 3;
		triangles[index][shapecount[index]].states = true;
		shapecount[index] += 1;
	}

#endif // Quiz8
#ifdef Quiz9
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		GLfloat size = randsize(gen);
		int index = 0;
		if (fx >= 0 && fy >= 0)
		{
			index = 0;
		}
		else if (fx < 0 && fy >= 0)
		{
			index = 1;
		}
		else if (fx < 0 && fy < 0)
		{
			index = 2;
		}
		else if (fx >= 0 && fy < 0)
		{
			index = 3;
		}

		triangles[index].shapecoord[0][0] = fx;
		triangles[index].shapecoord[0][1] = fy + sqrt(2) * size;
		triangles[index].shapecoord[1][0] = fx - size;
		triangles[index].shapecoord[1][1] = fy - size;
		triangles[index].shapecoord[2][0] = fx + size;
		triangles[index].shapecoord[2][1] = fy - size;

		GLfloat randR, randG, randB;
		RandomColor(randR, randG, randB);

		for (int j = 0; j < 3; ++j)
		{
			triangles[index].shapecolor[j][0] = randR;
			triangles[index].shapecolor[j][1] = randG;
			triangles[index].shapecolor[j][2] = randB;
		}

		origtriangles[index] = triangles[index];
	}

#endif // Quiz9
#ifdef Quiz10
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		points[0][0][0] = fx;
		points[0][0][1] = fy;
		for (int i = 1; i < spiralcount; ++i)
		{
			switch (i)
			{
			case 1:
				points[1][0][0] = fx + 0.4f;
				points[1][0][1] = fy + 0.4f;
				break;
			case 2:
				points[2][0][0] = fx - 0.4f;
				points[2][0][1] = fy + 0.4f;
				break;
			case 3:
				points[3][0][0] = fx - 0.4f;
				points[3][0][1] = fy - 0.4f;
				break;
			case 4:
				points[4][0][0] = fx + 0.4f;
				points[4][0][1] = fy - 0.4f;
				break;
			default:
				break;
			}
		}

		progress = 0;
		radius = 0;
		angle = 0;
		if (draw)
			RandomColor(bGCr, bGCg, bGCb);
		else
			draw = true;
	}
#endif // Quiz10
#ifdef Quiz12
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 14; i >= 0; --i)
		{
			if (fx >= shapes[i].left && fx <= shapes[i].right
				&& fy >= shapes[i].bottom && fy <= shapes[i].top)
			{
				drag = true;
				dragshape = i;
				prevX = fx;
				prevY = fy;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (drag)
		{
			drag = false;
		}
	}
#endif // Quiz12

}

GLvoid Timer(int value)
{
#ifdef Quiz9
	switch (movetype)
	{
	case DIAGONAL:
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				triangles[i].shapecoord[j][0] += moveX[i];
				triangles[i].shapecoord[j][1] += moveY[i];
			}

			if (CheckColideWall_Right(triangles[i]))
				moveX[i] = -0.05f;
			else if (CheckColideWall_Left(triangles[i]))
				moveX[i] = 0.05f;

			if (CheckColideWall_Top(triangles[i]))
				moveY[i] = -0.05f;
			else if (CheckColideWall_Bottom(triangles[i]))
				moveY[i] = 0.05f;
		}
		break;
	case ZIGZAG:
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				triangles[i].shapecoord[j][0] += moveX[i];
			}

			if (CheckColideWall_Right(triangles[i]))
			{
				moveX[i] = -0.05f;

				if (CheckColideWall_Top(triangles[i]))
					moveY[i] = -0.3f;
				else if (CheckColideWall_Bottom(triangles[i]))
					moveY[i] = 0.3f;

				for (int j = 0; j < 3; ++j)
				{
					triangles[i].shapecoord[j][1] += moveY[i];
				}
			}
			else if (CheckColideWall_Left(triangles[i]))
			{
				moveX[i] = 0.05f;

				if (CheckColideWall_Top(triangles[i]))
					moveY[i] = -0.1f;
				else if (CheckColideWall_Bottom(triangles[i]))
					moveY[i] = 0.1f;

				for (int j = 0; j < 3; ++j)
				{
					triangles[i].shapecoord[j][1] += moveY[i];
				}
			}
		}
		break;
	case RECTSPIRAL:
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				triangles[i].shapecoord[j][0] += moveX[i];
				triangles[i].shapecoord[j][1] += moveY[i];
			}

			if (moveX[i] > 0 && CheckColideWall_Right(triangles[i], 1.0f - limitX[0]))
			{
				moveX[i] = 0.0f;
				moveY[i] = 0.05f;
				limitX[0] += 0.1f;
			}
			else if (moveX[i] < 0 && CheckColideWall_Left(triangles[i], -1.0f + limitX[1]))
			{
				moveX[i] = 0.0f;
				moveY[i] = -0.05f;
				limitX[1] += 0.1f;
			}
			else if (moveY[i] > 0 && CheckColideWall_Top(triangles[i], 1.0f - limitY[0]))
			{
				moveX[i] = -0.05f;
				moveY[i] = 0.0f;
				limitY[0] += 0.1f;
			}
			else if (moveY[i] < 0 && CheckColideWall_Bottom(triangles[i], -1.0f + limitY[1]))
			{
				moveX[i] = 0.05f;
				moveY[i] = 0.0f;
				limitY[1] += 0.1f;
			}
		}
		break;
	case CIRCLESPIRAL:
		for (int i = 0; i < 4; ++i)
		{
			GLfloat x = (origtriangles[i].shapecoord[0][0] + origtriangles[i].shapecoord[1][0] + origtriangles[i].shapecoord[2][0]) / 3;
			GLfloat y = (origtriangles[i].shapecoord[0][1] + origtriangles[i].shapecoord[1][1] + origtriangles[i].shapecoord[2][1]) / 3;

			for (int j = 0; j < 3; ++j)
			{
				triangles[i].shapecoord[j][0] = radius * sin(angle * M_PI / 180)
					+ (origtriangles[i].shapecoord[j][0] - x) * cos(-angle * M_PI / 180)
					- (origtriangles[i].shapecoord[j][1] - y) * sin(-angle * M_PI / 180) + x;
				triangles[i].shapecoord[j][1] = radius * cos(angle * M_PI / 180)
					+ (origtriangles[i].shapecoord[j][0] - x) * sin(-angle * M_PI / 180)
					+ (origtriangles[i].shapecoord[j][1] - y) * cos(-angle * M_PI / 180) + y;
			}
			radius += 0.001f;
			angle += 1;
			if (angle >= 360)
				angle = 0;
		}
		break;
	default:
		break;
	}
#endif // Quiz9
#ifdef Quiz10
	if (draw)
	{
		for (int i = 0; i < spiralcount; ++i)
		{
			if (progress <= pointcount / 2)
			{
				points[i][progress][0] = points[i][0][0] + radius * cos(angle * M_PI / 180);
				points[i][progress][1] = points[i][0][1] + radius * sin(angle * M_PI / 180);

			}
			else if (progress < pointcount)
			{
				points[i][progress][0] = points[i][0][0] + 0.4f + radius * cos((angle + 180) * M_PI / 180);
				points[i][progress][1] = points[i][0][1] + radius * sin(angle * M_PI / 180);
			}
		}
		if (progress <= pointcount / 2)
		{
			radius += 0.2f / (pointcount / 2);
			angle += 1080 / (pointcount / 2);
			++progress;
		}
		else if (progress < pointcount)
		{
			radius -= 0.2f / (pointcount - (pointcount / 2));
			angle += 1080 / (pointcount - (pointcount / 2));
			++progress;
		}
	}
#endif // Quiz10
#ifdef Quiz11
	shapes[0].shapecoord[2][1] = (1 - timer) * 0.74f + timer * 0.25f;

	shapes[1].shapecoord[3][0] = (1 - timer) * 0.75f + timer * 0.25f;

	shapes[2].shapecoord[0][0] = (1 - timer) * -0.75f + timer * -0.6f;
	shapes[2].shapecoord[1][0] = (1 - timer) * -0.25f + timer * -0.4f;
	shapes[2].shapecoord[3][1] = (1 - timer) * -0.5f + timer * -0.25f;

	shapes[3].shapecoord[1][0] = (1 - timer) * 0.6f + timer * 0.405;
	shapes[3].shapecoord[2][0] = (1 - timer) * 0.75f + timer * 0.505f;
	shapes[3].shapecoord[2][1] = (1 - timer) * -0.5f + timer * -0.25f;
	shapes[3].shapecoord[4][0] = (1 - timer) * 0.25f + timer * 0.5f;
	shapes[3].shapecoord[4][1] = (1 - timer) * -0.5f + timer * -0.25f;

	if (timer < 1.0f)
		timer += 0.01f;
	else
		timer = 1.0f;
#endif // Quiz11

	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, Timer, 1);
}

void Motion(int x, int y)
{
	float fx = 0.0, fy = 0.0;
	convertXY(x, y, fx, fy);

#ifdef Quiz12
	if (drag)
	{
		for (int i = 0; i < shapes[dragshape].points; ++i)
		{
			shapes[dragshape].shapecoord[i][0] += fx - prevX;
			shapes[dragshape].shapecoord[i][1] += fy - prevY;
		}
		shapes[dragshape].left += fx - prevX;
		shapes[dragshape].right += fx - prevX;
		shapes[dragshape].top += fy - prevY;
		shapes[dragshape].bottom += fy - prevY;
		prevX = fx;
		prevY = fy;
	}
#endif // Quiz12

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
#ifdef Quiz7
	glBufferData(GL_ARRAY_BUFFER, SHAPES * 12 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
#endif // Quiz7
#ifdef Quiz8
	glBufferData(GL_ARRAY_BUFFER, SHAPES * 4 * 12 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
#endif // Quiz8
#ifdef Quiz9
	glBufferData(GL_ARRAY_BUFFER, 4 * 12 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
#endif // Quiz9
#ifdef Quiz10
	glBufferData(GL_ARRAY_BUFFER, pointcount * 5 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
#endif // Quiz10
#ifdef Quiz11
	glBufferData(GL_ARRAY_BUFFER, 4 * 18 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
#endif // Quiz11
#ifdef Quiz12
	glBufferData(GL_ARRAY_BUFFER, 23 * 18 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
#endif // Quiz12


	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float

#ifdef Quiz7
	glBufferData(GL_ARRAY_BUFFER, SHAPES * 12 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	GLfloat colors[4][3];
	for (int i = 0; i < SHAPES; i++)
	{
		for (int z = 0; z < 4; z++)
		{
			colors[z][0] = 0;
			colors[z][1] = 0;
			colors[z][2] = 0;
		}
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), colors);
	}
#endif // Quiz7
#ifdef Quiz8
	glBufferData(GL_ARRAY_BUFFER, SHAPES * 4 * 12 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	GLfloat colors[4][3];
	for (int i = 0; i < SHAPES * 4; i++)
	{
		for (int z = 0; z < 4; z++)
		{
			colors[z][0] = 0;
			colors[z][1] = 0;
			colors[z][2] = 0;
		}
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), colors);
	}
#endif // Quiz8
#ifdef Quiz9
	glBufferData(GL_ARRAY_BUFFER, 4 * 12 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	GLfloat colors[4][3];
	for (int i = 0; i < 4; i++)
	{
		for (int z = 0; z < 4; z++)
		{
			colors[z][0] = 0;
			colors[z][1] = 0;
			colors[z][2] = 0;
		}
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), colors);
	}
#endif // Quiz9
#ifdef Quiz10
	glBufferData(GL_ARRAY_BUFFER, pointcount * 5 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	GLfloat colors[3];
	for (int i = 0; i < pointcount * 5; i++)
	{
		colors[0] = 0;
		colors[1] = 0;
		colors[2] = 0;

		glBufferSubData(GL_ARRAY_BUFFER, i * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), colors);
	}
#endif // Quiz10
#ifdef Quiz11
	glBufferData(GL_ARRAY_BUFFER, 4 * 18 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	GLfloat colors[6][3];
	for (int i = 0; i < 4; i++)
	{
		for (int z = 0; z < 6; z++)
		{
			colors[z][0] = 0;
			colors[z][1] = 0;
			colors[z][2] = 0;
		}
		glBufferSubData(GL_ARRAY_BUFFER, i * 18 * sizeof(GLfloat), 18 * sizeof(GLfloat), colors);
	}
#endif // Quiz11
#ifdef Quiz12
	glBufferData(GL_ARRAY_BUFFER, 23 * 18 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
#endif // Quiz12

	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

#ifdef Quiz7
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[SHAPES * 4];
	for (int i = 0; i < SHAPES; i++)
	{
		indices[i * 4] = i * 4;
		indices[i * 4 + 1] = i * 4 + 1;
		indices[i * 4 + 2] = i * 4 + 2;
		indices[i * 4 + 3] = i * 4 + 3;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif // Quiz7
#ifdef Quiz8
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[SHAPES * 4 * 4];
	for (int i = 0; i < SHAPES * 4; i++)
	{
		indices[i * 4] = i * 4;
		indices[i * 4 + 1] = i * 4 + 1;
		indices[i * 4 + 2] = i * 4 + 2;
		indices[i * 4 + 3] = i * 4 + 3;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif // Quiz8
#ifdef Quiz9
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[4 * 4];
	for (int i = 0; i < 4; i++)
	{
		indices[i * 4] = i * 4;
		indices[i * 4 + 1] = i * 4 + 1;
		indices[i * 4 + 2] = i * 4 + 2;
		indices[i * 4 + 3] = i * 4 + 3;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif // Quiz9
#ifdef Quiz10
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[pointcount * 5];
	for (int i = 0; i < pointcount * 5; i++)
	{
		indices[i] = i;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif // Quiz10
#ifdef Quiz11
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[4 * 6];
	for (int i = 0; i < 24; i++)
	{
		indices[i] = i;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif // Quiz11
#ifdef Quiz12
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint indices[23 * 6];
	for (int i = 0; i < 23 * 6; i++)
	{
		indices[i] = i;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif // Quiz12

}

void UpdateBuffer()
{
#ifdef Quiz7
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < SHAPES; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), shape[i].shapecoord);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < SHAPES; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), shape[i].shapecolor);
	}
#endif Quiz7
#ifdef Quiz8
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < SHAPES; ++j)
			glBufferSubData(GL_ARRAY_BUFFER, (i * SHAPES + j) * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), triangles[i][j].shapecoord);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < SHAPES; ++j)
			glBufferSubData(GL_ARRAY_BUFFER, (i * SHAPES + j) * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), triangles[i][j].shapecolor);
	}
#endif Quiz8
#ifdef Quiz9
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), triangles[i].shapecoord);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), 12 * sizeof(GLfloat), triangles[i].shapecolor);
	}
#endif Quiz9
#ifdef Quiz10
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < pointcount; ++j)
		{
			glBufferSubData(GL_ARRAY_BUFFER, (i * pointcount + j) * 3 * sizeof(GLfloat), 3 * sizeof(GLfloat), points[i][j]);
		}
	}
#endif // Quiz10
#ifdef Quiz11
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 18 * sizeof(GLfloat), 18 * sizeof(GLfloat), shapes[i].shapecoord);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 18 * sizeof(GLfloat), 18 * sizeof(GLfloat), shapes[i].shapecolor);
	}
#endif Quiz11
#ifdef Quiz12
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	for (int i = 0; i < 15; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 18 * sizeof(GLfloat), 18 * sizeof(GLfloat), shapes[i].shapecoord);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	for (int i = 0; i < 15; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i * 18 * sizeof(GLfloat), 18 * sizeof(GLfloat), shapes[i].shapecolor);
	}
#endif Quiz12

}