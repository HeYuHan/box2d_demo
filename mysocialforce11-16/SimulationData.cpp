#include "SimulationData.h"
SimulationData::SimulationData()
{
}

SimulationData::~SimulationData()
{
}

void SimulationData::FormatData(ifstream & input_stream)
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
	}
	cout << all_frame_datas.size();
}

bool SimulationData::Init()
{
	return true;
}

void SimulationData::Exit()
{

}

void SimulationData::Update(float frame_time)
{

}

void SimulationData::Draw()
{

}

void SimulationData::OnKeyDown(unsigned char key, int x, int y)
{

}
