#pragma once
#include <glm/glm/glm.hpp>
#include "randomcolor.h"

static const int MAX_POINTS = 20;  // 변경 가능한 최대 점 수

uniform_real_distribution<float> randspeedX(-0.035f, 0.035f);
uniform_real_distribution<float> randspeedY(0.0f, 0.004f);

class Shape
{
public:
	glm::vec3 shapecoord[MAX_POINTS];
	glm::vec3 shapecolor[MAX_POINTS];
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 revolution;
	glm::vec3 scaling;
	float l, b, r, t;
	float speedX, speedY;
	int points;

	Shape()
	{
		for (int i = 0; i < MAX_POINTS; ++i)
		{
			shapecoord[i].x = 0.0f;
			shapecoord[i].y = 0.0f;
			shapecoord[i].z = 0.0f;
			shapecolor[i].r = 0.0f;
			shapecolor[i].g = 0.0f;
			shapecolor[i].b = 0.0f;
		}
		translation = glm::vec3(0.0);
		rotation = glm::vec3(0.0);
		revolution = glm::vec3(0.0);
		scaling = glm::vec3(0.0);
		l = FLT_MAX; b = FLT_MAX; r = FLT_MIN; t = FLT_MIN;
		speedX = 0.0f; speedY = 0.0f;
		points = 0;
	}

	Shape(int state, glm::vec3 coord[])
	{
		if (state > MAX_POINTS)
			exit(6974);

		glm::vec3 color;
		RandomColor(color.r, color.g, color.b);

		for (int i = 0; i < state; ++i)
		{
			shapecoord[i] = coord[i];
			shapecolor[i] = color;

			if (coord[i].x < l)
				l = coord[i].x;
			if (coord[i].x > r)
				r = coord[i].x;
			if (coord[i].y < b)
				b = coord[i].x;
			if (coord[i].y > t)
				t = coord[i].x;
		}
		translation = glm::vec3(0.0);
		rotation = glm::vec3(0.0);
		revolution = glm::vec3(0.0);
		scaling = glm::vec3(1.0);
		speedX = randspeedX(gen); speedY = randspeedX(gen);
		points = state;
	}

	Shape(int state, glm::vec3 coord[], glm::vec3 color)
	{
		if (state > MAX_POINTS)
			exit(6974);

		for (int i = 0; i < state; ++i)
		{
			shapecoord[i] = coord[i];
			shapecolor[i] = color;

			if (coord[i].x < l)
				l = coord[i].x;
			if (coord[i].x > r)
				r = coord[i].x;
			if (coord[i].y < b)
				b = coord[i].x;
			if (coord[i].y > t)
				t = coord[i].x;
		}
		translation = glm::vec3(0.0);
		rotation = glm::vec3(0.0);
		revolution = glm::vec3(0.0);
		scaling = glm::vec3(1.0);
		speedX = randspeedX(gen); speedY = randspeedX(gen);
		points = state;
	}

	~Shape() {}

	float Left() { return l * scaling.x + translation.x; }
	float Bottom() { return b * scaling.y + translation.y; }
	float Right() { return r * scaling.x + translation.x; }
	float Top() { return t * scaling.y + translation.y; }

	void MovebyTime(float gravity)
	{
		translation.x += speedX;
		translation.y += speedY;
		speedY += gravity;
	}

	void Draw(int i, GLenum mode = GL_TRIANGLE_STRIP)
	{
		glDrawArrays(mode, i * MAX_POINTS, points);
	}
};
