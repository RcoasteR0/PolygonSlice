#pragma once
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());

void RandomColor(float& colorR, float& colorG, float& colorB)
{
	uniform_real_distribution<float> random(0, 1);
	colorR = random(gen);
	colorG = random(gen);
	colorB = random(gen);
}
