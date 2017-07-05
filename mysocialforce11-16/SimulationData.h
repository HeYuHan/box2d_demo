#pragma once
#include "SimulationBase.h"
#include <unordered_map>
#include "json/json.h"
struct FrameData
{
	int size;
	int id;
	int frame_start;
	b2Vec2* data;
};
class SimulationData:public SimulationBase
{
public:
	SimulationData();
	~SimulationData();
	bool FormatData(ifstream &input_stream);
	virtual bool Init();
	virtual void Exit();
	virtual void Update(float frame_time);
	virtual void Draw();
	virtual void OnKeyDown(unsigned char key, int x, int y);
public:
	float play_rate;
private:
	unordered_map<int, FrameData*> all_frame_datas;
	float frame_count;
	
};

