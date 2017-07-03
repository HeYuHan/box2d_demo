#pragma once
#ifndef SimulationThread_h
#define SimulationThread_h
#include "SimulationBase.h"
using namespace std;
typedef enum
{
	L_None = 1 << 0,
	L_Ground = 1 << 1,
	L_Player = 1 << 2
}LayerMask;
typedef enum
{
	S_BOX = 1, S_CRICLE, S_POLYGON, S_LINE, S_OBSTACLE
}ShapeType;
#define GLOBAL_FORCE 20
//最大生存时间
#define MAX_LIFE_TIME  30
//最大受力
#define MAX_FORCE  242
//最大速度
#define MAX_VVELOCITY  50
static int GLOBAL_SHAPE_ID = 0;
struct  WayInfo
{
	int point_count;
	b2Vec2* path;

};
class ShapeInfo :public b2RayCastCallback
{
public:
	int id;
	ShapeType type;
	float r;
	float width, height;
	b2Vec2 points[100];
	int pointcount;
	b2Body* body;
	b2Vec3 color;
	WayInfo* way;
	b2Vec2 startPosition;
	b2Vec2 nextPosition;
	float force;
	bool moveFinish;
	float socialforce;
	bool hitGround;
	float lifeTime;
	float current_force;
	SimulationBase* simulation;

	ShapeInfo();
	//virtual float32 b2RayCastCallback(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float32 fraction);
	//物理碰撞的回调函数
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	b2Vec2 GetPosition();
	b2Vec2 GetVelocity();
	void Draw();
	void DrawObstacle();
	void DrawCricle();
	void DrawBox();
	void UpdateNetPosition();
	void InitStartPostion()
	{
		if (body)
		{
			b2Vec2 pos = way->path[0];
			body->SetTransform(pos, 0);
			UpdateNetPosition();
			b2Vec2 dir = nextPosition - way->path[0];
			dir.Normalize();
			body->SetLinearVelocity(10 * dir);
		}
	}
	void Update(float dt);
	/*void OutPutInfoToFile()
	{
		b2Vec2 pos = body->GetPosition();
		out_file << "" << id << " " << pos.x << " " << pos.y << " " << lifeTime << "  " << current_force << endl;
	}*/
};
class SimulationSocialForce:public SimulationBase
{
public:
	SimulationSocialForce();
	virtual bool Init();
	virtual void Exit();
	virtual void Update(float frame_time);
	virtual void Draw();
	void InitWorld();
	void InitGround();
	void InitObstacle();
	void CrateObstacle(ShapeInfo *info);
	void CrateObstacle(b2Vec2 point1, b2Vec2 point2);
	b2Vec2 SocialForce(ShapeInfo* infos);
	ShapeInfo* NewShapeInfo();
	void RemoveShapeInfo(ShapeInfo* info);
	virtual void OnKeyDown(unsigned char key, int x, int y);
	//构建一个有至少3个点组成的一个封闭图形
	b2Body* CreatePolygonShape(b2Vec2 *points, int count);
	b2Body* CreateBox(ShapeInfo* info);
	b2Body* CreateCircle(float r);
private:
	vector<ShapeInfo*> all_shapeinfo;
	b2Body* Ground; //创建地面
	float32 Gravity; //同一平面，重力设为0
};
extern b2World* gWorld;
#endif // !SimulationThread_h
