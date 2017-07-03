#include "SimulationBase.h"
#include"gl/glut.h"
SimulationBase::SimulationBase():
	m_SenceWidth(400),
	m_SenceHeight(400)
{
}

SimulationBase::~SimulationBase()
	
{
}
void SimulationBase::OutPut(std::fstream & out_file)
{
}
void SimulationBase::OffsetDraw(float x, float y)
{
	x /= m_SenceWidth;
	y /= m_SenceHeight;
	x -= 0.5f;
	y -= 0.5f;
	x *= 2;
	y *= 2;
	glVertex2f(x, y);
}
void SimulationBase::OffsetDraw(b2Vec2 pos)
{
	OffsetDraw(pos.x, pos.y);
}
b2Vec2 SimulationBase::RotateVector(b2Vec2 pos, b2Vec2 center, float angle)
{
	b2Vec2 dir = pos - center;
	float cx = dir.x*cos(angle) - dir.y*sin(angle);
	float cy = dir.x*sin(angle) + dir.y*cos(angle);
	dir.x = cx;
	dir.y = cy;
	return dir + center;
}

void SimulationBase::OnKeyDown(unsigned char key, int x, int y)
{
}
int random(int a, int b)
{
	return (rand() % (b - a)) + a;
}
float random(float a, float b)
{
	float x = (float)rand() / RAND_MAX;
	return random((int)a, (int)b) + x;
}
b2Vec3 RandomColor()
{
	return b2Vec3(random(0.0f, 1.0f), random(0.0f, 1.0f), random(0.0f, 1.0f));
}