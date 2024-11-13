#pragma once
#include <glm/glm/glm.hpp>
#include "randomcolor.h"

static const int MAX_POINTS = 500;  // 변경 가능한 최대 점 수

class Shape
{
public:
	glm::vec3 shapecoord[MAX_POINTS];
	glm::vec3 shapecolor[MAX_POINTS];
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 revolution;
	glm::vec3 scaling;
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
		}
		translation = glm::vec3(0.0);
		rotation = glm::vec3(0.0);
		revolution = glm::vec3(0.0);
		scaling = glm::vec3(1.0);
		points = state;
	}

	Shape(int state, glm::vec3 coord[], glm::vec3 color)
	{
		if (state > MAX_POINTS)
			exit(6974);

		for (int i = 0; i < state; ++i)
		{
			shapecoord[i].x = 0.0f;
			shapecoord[i].y = 0.0f;
			shapecoord[i].z = 0.0f;
			shapecolor[i].r = 0.0f;
			shapecolor[i].g = 0.0f;
			shapecolor[i].b = 0.0f;
			shapecoord[i] = coord[i];
			shapecolor[i] = color;
		}
		translation = glm::vec3(0.0);
		rotation = glm::vec3(0.0);
		revolution = glm::vec3(0.0);
		scaling = glm::vec3(1.0);
		points = state;
	}

	~Shape() {}

	void Draw(int i, GLenum mode = GL_TRIANGLE_FAN)
	{
		glDrawArrays(mode, i * MAX_POINTS, points);
	}
	
	void DrawLineStrip(int i)
	{
		glDrawArrays(GL_LINE_STRIP, i * MAX_POINTS, points);
	}

	void DrawCylinder(int i)
	{
		int baseVertexCount = points / 3;

		// 하단 밑면
		glDrawArrays(GL_TRIANGLE_FAN, i * MAX_POINTS, baseVertexCount);

		// 상단 밑면
		glDrawArrays(GL_TRIANGLE_FAN, i * MAX_POINTS + baseVertexCount, baseVertexCount);

		// 측면
		glDrawArrays(GL_TRIANGLE_STRIP, i * MAX_POINTS + 2 * baseVertexCount, points - 2 * baseVertexCount);
	}
};
