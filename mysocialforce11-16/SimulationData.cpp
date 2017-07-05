#include "SimulationData.h"
SimulationData::SimulationData():
	frame_count(0),
	play_rate(1)
{
}

SimulationData::~SimulationData()
{
}

bool SimulationData::FormatData(ifstream & input_stream)
{
	if (!input_stream.fail())
	{
		string line;
		while (getline(input_stream, line))
		{
			string_replace(line, ")(", ",");
			string_replace(line, "(", ",");
			string_replace(line, ")", "]");
			line = "[" + line;
			Json::Value Root;
			Json::Reader Reader;
			if (Reader.parse(line, Root))
			{
				if (Root.isArray())
				{
					int id = Root[(Json::UInt)0].asInt();
					FrameData* fd = all_frame_datas[id];
					if (fd)
					{
						delete[] fd->data;
						delete fd;
					}
					fd=new FrameData;
					
					all_frame_datas[id] = fd;
					int data_size = Root.size() - 1;
					fd->id = id;
					fd->size = data_size / 3 * 2;
					fd->data = new b2Vec2[fd->size];
					int data_index = 0;
					for (int i = 1;i < data_size;)
					{
						int x = Root[i++].asInt();
						int y = Root[i++].asInt();
						fd->data[data_index++] = b2Vec2(x, y);
						if (i == 3)
						{
							fd->frame_start = Root[i++].asInt();
						}
						else
						{
							i++;
						}
					}
					
				}
			}
		}
		return true;
	}
	return false;
}

bool SimulationData::Init()
{
	frame_count = 0;
	

	return true;
}

void SimulationData::Exit()
{
	for (auto data : all_frame_datas)
	{
		delete[] data.second->data;
		delete data.second;
	}
	all_frame_datas.clear();
}

void SimulationData::Update(float frame_time)
{
	frame_count += frame_time*play_rate;
}

void SimulationData::Draw()
{
	for (auto it : all_frame_datas)
	{
		FrameData *data = it.second;
		int play_frame = frame_count;
		int play_index = play_frame - data->frame_start;
		int unplay = data->size - (play_frame -data->frame_start);
		
		if (play_index >0 && play_index < data->size)
		{
			b2Vec2 pos = data->data[play_index];
			glColor3f(1, 1, 1);
			glBegin(GL_LINE_LOOP);
			int iter = 20;
			int r = 10;
			for (int i = 0; i < iter; ++i)
				OffsetDraw(pos.x + r*cos(2 * PI / iter * i), pos.y + r*sin(2 * PI / iter * i));
			glEnd();
		}
	}
	
}

void SimulationData::OnKeyDown(unsigned char key, int x, int y)
{

}
