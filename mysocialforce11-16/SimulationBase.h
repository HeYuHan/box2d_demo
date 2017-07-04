#pragma once
#ifndef SimulationBase_h
#define SimulationBase_h
#include "pch.h"
class ShapeInfo;
class SimulationBase
{
public:
	SimulationBase();
	~SimulationBase();
	virtual void Draw() = 0;
	virtual void Update(float frame_time) = 0;
	virtual bool Init() = 0;
	virtual void Exit() = 0;
	virtual void OutPut(std::fstream &out_file);
	void OffsetDraw(float x, float y);
	void OffsetDraw(b2Vec2 pos);
	b2Vec2 RotateVector(b2Vec2 pos, b2Vec2 center, float angle);
	virtual void OnKeyDown(unsigned char key, int x, int y);
public:
	float m_SenceHeight;
	float m_SenceWidth;
private:

};
int random(int a, int b);
float random(float a, float b);
b2Vec3 RandomColor();
void string_replace(string&s1, const string&s2, const string&s3);
#endif // !SimulationBase_h
