#include "SimulationSocialForce.h"
#include "Box2D/Box2D.h"
#include "GL/glut.h"
b2World* gWorld = nullptr;

ShapeInfo::ShapeInfo()
{
	type = S_BOX;
	r = 0;
	width = height = 0;
	body = nullptr;
	pointcount = 0;
	color = b2Vec3(1, 1, 1);
	force = GLOBAL_FORCE;
	moveFinish = false;
	id = GLOBAL_SHAPE_ID;
	GLOBAL_SHAPE_ID++;
	socialforce = 1;
	lifeTime = 0;
	current_force = 0;
}
float32 ShapeInfo::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	auto body = fixture->GetBody();
	if (body)
	{
		auto data = fixture->GetFilterData();
		hitGround = data.categoryBits == L_Ground;
	}
	return 0;
};
b2Vec2 ShapeInfo::GetPosition()
{
	if (body)

	{
		return body->GetPosition();
	}
	return b2Vec2(0, 0);
}
b2Vec2 ShapeInfo::GetVelocity()
{
	if (body)
	{
		return body->GetLinearVelocity();
	}
	return b2Vec2(0, 0);
}
void ShapeInfo::Draw()
{
	switch (type)
	{
	case S_BOX:
		DrawBox();
		break;
	case S_CRICLE:
		DrawCricle();
		break;
	case S_POLYGON:
		break;
	case S_OBSTACLE:
		DrawObstacle();
	default:
		break;
	}
}
void ShapeInfo::DrawObstacle()
{
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_LINES);
	for (int i = 0; i < 2; i++)
	{
		simulation->OffsetDraw(points[i]);
	}
	glEnd();
}
void ShapeInfo::DrawCricle()
{
	if (!body)return;
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_LINE_LOOP);
	b2Vec2 pos = body->GetPosition();
	int iter = 20;
	for (int i = 0; i < iter; ++i)
		simulation->OffsetDraw(pos.x + r*cos(2 * PI / iter * i), pos.y + r*sin(2 * PI / iter * i));
	glEnd();
	/*glColor3f(0, 0, 0);
	b2Vec2 p = pos;
	char pos_str_x[10] = { 0 };
	sprintf(pos_str_x, "x:%d", (int)(p.x));
	p.x -= r - 4;
	p.y += 2;
	p = getScreenPosition(p);
	glRasterPos2f(p.x, p.y);
	DrawString(pos_str_x);

	p = pos;
	char pos_str_y[10] = { 0 };
	sprintf(pos_str_y, "y:%d", (int)(p.y));
	p.x -= r - 4;
	p.y -= 6;
	p = getScreenPosition(p);
	glRasterPos2f(p.x, p.y);
	DrawString(pos_str_y);*/
}
void ShapeInfo::DrawBox()
{
	if (!body)return;
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_QUADS);
	float halfw = width*0.5f;
	float halfh = height*0.5f;
	b2Vec2 pos = body->GetPosition();
	float angle = body->GetAngle();
	b2Vec2 p1 = simulation->RotateVector(b2Vec2(-halfw, -halfh), b2Vec2(0, 0), angle);
	b2Vec2 p2 = simulation->RotateVector(b2Vec2(-halfw, halfh), b2Vec2(0, 0), angle);
	b2Vec2 p3 = simulation->RotateVector(b2Vec2(halfw, halfh), b2Vec2(0, 0), angle);
	b2Vec2 p4 = simulation->RotateVector(b2Vec2(halfw, -halfh), b2Vec2(0, 0), angle);
	simulation->OffsetDraw(p1 + pos);
	simulation->OffsetDraw(p2 + pos);
	simulation->OffsetDraw(p3 + pos);
	simulation->OffsetDraw(p4 + pos);
	glEnd();
}
void ShapeInfo::UpdateNetPosition()
{
	if (body)
	{
		//定点射线检测，总是以最靠近终点的点为期望的移动方向，
		//通过射线检测可判定当前位置到期望位置以直线行走是否会碰到障碍物
		//未碰到说明该路径可行，不能则迭代下一个路径
		b2Vec2 p = body->GetPosition();
		hitGround = true;
		for (int i = way->point_count - 1; i > 0; i--)
		{
			if (hitGround && (p - way->path[i]).LengthSquared() > 0)
			{
				nextPosition = way->path[i];
				hitGround = false;
				gWorld->RayCast(this, p, way->path[i]);

			}

		}
	}
}

void ShapeInfo::Update(float dt)
{

	if (body)
	{

		b2Vec2 pos = body->GetPosition();
		UpdateNetPosition();
		b2Vec2 target_pos = nextPosition;

		//v0
		b2Vec2 current_v = body->GetLinearVelocity();
		//速度方向
		b2Vec2 dir = target_pos - pos;
		dir.Normalize();
		b2Vec2 f = force*dir;
		//社会力
		SimulationSocialForce *ssf = static_cast<SimulationSocialForce*>(simulation);
		if (ssf)
		{
			f += socialforce*ssf->SocialForce(this);
		}
		current_force = f.Length();
		//v=at+v0
		//a=f/m
		//m=1,v=ft+v0;
		b2Vec2 set_v = current_v + dt*f;
		if (set_v.Length() > MAX_VVELOCITY)
		{
			set_v.Normalize();
			set_v *= MAX_VVELOCITY;
		}
		body->SetLinearVelocity(set_v);
		if (pos.y < 20)
		{
			moveFinish = true;
		}
		if ((lifeTime += dt) > MAX_LIFE_TIME)
		{
			moveFinish = true;
		}
		if (current_force > MAX_FORCE)
		{
			moveFinish = true;
		}
		//if (moveFinish)
		/*{
			OutPutInfoToFile();
		}*/
	}
}
//////////////////////////////////////////////////////////////////////////////
const int PATH_COUNT = 3;

b2Vec2 WayOne[] = { { 50, 400 },{ 200, 350 },{ 350, 300 },{ 350, 100 },{ 350, 0 } };
b2Vec2 WayTwo[] = { { 650, 360 },{ 500, 320 },{ 370, 300 },{ 370, 100 },{ 320, 0 } };
b2Vec2 WayThree[] = { b2Vec2(0, 360), b2Vec2(200, 300), b2Vec2(370, 300),  b2Vec2(320, 0) };
WayInfo Ways[PATH_COUNT] = {
	{ 4, WayOne },
	{ 4, WayTwo },
	{ 4, WayThree },
};
//////////////////////////////////////////////////////////////////////////////
bool SimulationSocialForce::Init()
{
	InitWorld();
	InitGround();
	InitObstacle();
	return true;
}
void SimulationSocialForce::Exit()
{
	if (gWorld)
	{
		delete gWorld;
		gWorld = nullptr;
	}
	for (auto shape : all_shapeinfo)
	{
		delete shape;
	}
	all_shapeinfo.clear();
}
SimulationSocialForce::SimulationSocialForce() :Ground(nullptr), Gravity(0)
{

}
void SimulationSocialForce::InitWorld()
{
	gWorld = new b2World(b2Vec2(0, Gravity));
	gWorld->SetAllowSleeping(true);
	gWorld->SetContinuousPhysics(true);
}

void SimulationSocialForce::InitGround()
{
	// 底部  
	CrateObstacle(b2Vec2(0, 0), b2Vec2(m_SenceWidth, 0));
	// 顶部  
	CrateObstacle(b2Vec2(0, m_SenceHeight), b2Vec2(m_SenceWidth, m_SenceHeight));
	// 左边  
	CrateObstacle(b2Vec2(0, m_SenceHeight), b2Vec2(0, 0));
	// 右边  
	CrateObstacle(b2Vec2(m_SenceWidth, m_SenceHeight), b2Vec2(m_SenceWidth, 0));
}
ShapeInfo* SimulationSocialForce::NewShapeInfo()
{
	ShapeInfo *result = new ShapeInfo;
	result->simulation = this;
	all_shapeinfo.push_back(result);
	return result;
}
void SimulationSocialForce::RemoveShapeInfo(ShapeInfo* info)
{
	vector<ShapeInfo*>::iterator iter = find(all_shapeinfo.begin(), all_shapeinfo.end(), info);
	if (iter != all_shapeinfo.end())
	{
		all_shapeinfo.erase(iter);
		if (info->body)
		{
			gWorld->DestroyBody(info->body);
		}

		delete info;
	}
}
void SimulationSocialForce::InitObstacle()
{
	//可以更改颜色,找出是哪一根线(r,g,b)
	auto info = NewShapeInfo();
	info->type = S_OBSTACLE;
	info->color = b2Vec3(1, 0, 0);
	info->points[0] = b2Vec2(0, 450);
	info->points[1] = b2Vec2(700, 450);
	CrateObstacle(info);

	info = NewShapeInfo();
	info->type = S_OBSTACLE;
	info->color = b2Vec3(0, 1, 0);
	info->points[0] = b2Vec2(0, 250);
	info->points[1] = b2Vec2(325, 250);
	CrateObstacle(info);

	info = NewShapeInfo();
	info->type = S_OBSTACLE;
	info->color = b2Vec3(0, 0, 1);
	info->points[0] = b2Vec2(325, 250);
	info->points[1] = b2Vec2(325, 0);
	CrateObstacle(info);

	info = NewShapeInfo();
	info->type = S_OBSTACLE;
	info->color = b2Vec3(1, 1, 0);
	info->points[0] = b2Vec2(375, 0);
	info->points[1] = b2Vec2(375, 250);
	CrateObstacle(info);

	info = NewShapeInfo();
	info->type = S_OBSTACLE;
	info->color = b2Vec3(1, 0, 1);
	info->points[0] = b2Vec2(375, 250);
	info->points[1] = b2Vec2(700, 250);
	CrateObstacle(info);
}
void SimulationSocialForce::Update(float frame_time)
{
	if (gWorld)
	{
		for (int i = 0; i < all_shapeinfo.size(); i++)
		{
			all_shapeinfo[i]->Update(frame_time);
			if (all_shapeinfo[i]->moveFinish)
			{
				RemoveShapeInfo(all_shapeinfo[i]);
				i--;
			}
		}
		gWorld->Step(frame_time, 1, 1);
	}
}
void SimulationSocialForce::Draw()
{
	for (int i = 0; i < all_shapeinfo.size(); i++)
	{
		all_shapeinfo[i]->Draw();
	}
}
void SimulationSocialForce::CrateObstacle(ShapeInfo *info)
{
	CrateObstacle(info->points[0], info->points[1]);
}
void SimulationSocialForce::CrateObstacle(b2Vec2 point1, b2Vec2 point2)
{
	//地面物体定义  
	b2BodyDef groundBodyDef;
	//左下角  
	groundBodyDef.type = b2_staticBody;
	//创建地面物体  
	if (!Ground)
	{
		Ground = gWorld->CreateBody(&groundBodyDef);
	}
	//定义一个有边的形状  
	b2EdgeShape groundBox;
	b2Filter filter;
	filter.categoryBits = L_Ground;
	filter.maskBits = L_Player | L_Ground;
	// 底部  
	groundBox.Set(point1, point2);
	//使用夹具固定形状到物体上  
	auto fixture = Ground->CreateFixture(&groundBox, 0);
	fixture->SetFilterData(filter);
}
double DirAngle(b2Vec2 &dir1, b2Vec2 &dir2)
{
	double angleThis = atan2(dir1.x, dir1.y);
	double angleOther = atan2(dir2.x, dir2.y);

	// compute angle
	double diffAngle = angleOther - angleThis;
	// → normalize angle
	if (diffAngle > PI) diffAngle -= 2 * PI;
	else if (diffAngle <= -PI) diffAngle += 2 * PI;

	return diffAngle;
}
b2Vec2 SimulationSocialForce::SocialForce(ShapeInfo* infos)
{
	if (!infos || !infos->body)return b2Vec2(0, 0);
	// define relative importance of position vs velocity vector
	// (set according to Moussaid-Helbing 2009)
	const double lambdaImportance = 2.0;


	// define speed interaction
	// (set according to Moussaid-Helbing 2009)
	const double gamma = 0.35;

	// define speed interaction
	// (set according to Moussaid-Helbing 2009)
	const double n = 2;

	// define angular interaction
	// (set according to Moussaid-Helbing 2009)
	const double n_prime = 3;

	b2Vec2 force(0, 0);
	for (ShapeInfo* other : all_shapeinfo) {
		// don't compute social force to yourself
		if (other->id == infos->id&&other->body) continue;

		// compute difference between both agents' positions
		b2Vec2 diff = other->GetPosition() - infos->GetPosition();

		// skip futher computation if they are too far away from each
		// other. Should speed up things.
		if (diff.LengthSquared() > 64.0) continue; // val to high --chgloor 20160630

		b2Vec2 diffDirection = diff;
		diffDirection.Normalize();
		// compute difference between both agents' velocity vectors
		// Note: the agent-other-order changed here
		b2Vec2 velDiff = infos->GetVelocity() - other->GetVelocity();

		// compute interaction direction t_ij
		b2Vec2 interactionVector = lambdaImportance * velDiff + diffDirection;
		double interactionLength = interactionVector.Length();
		b2Vec2 interactionDirection = b2Vec2(interactionVector.x / interactionLength, interactionVector.y / interactionLength);

		// compute angle theta (between interaction and position difference vector)
		double theta = DirAngle(interactionDirection, diffDirection);
		int thetaSign = (theta == 0) ? (0) : (theta / abs(theta));

		// compute model parameter B = gamma * ||D||
		double B = gamma * interactionLength;

		// According to paper, this should be the sum of the two forces...
		//          force += -exp(-diff.length()/B)
		//              * (exp(-pow(n_prime*B*theta,2)) * interactionDirection
		//                  + exp(-pow(n*B*theta,2)) * interactionDirection.leftNormalVector());

		double forceVelocityAmount = -exp(-diff.Length() / B - (n_prime*B*theta)*(n_prime*B*theta));
		double forceAngleAmount = -thetaSign * exp(-diff.Length() / B - (n*B*theta)*(n*B*theta));

		b2Vec2 forceVelocity = forceVelocityAmount * interactionDirection;
		b2Vec2 leftNormalVector = b2Vec2(-interactionDirection.y, interactionDirection.x);
		b2Vec2 forceAngle = forceAngleAmount * leftNormalVector;

		force += forceVelocity + forceAngle;
	}
	return force;
}
void SimulationSocialForce::OnKeyDown(unsigned char key, int x, int y)
{
	int select_way_index = random(0, PATH_COUNT);
	WayInfo* way_info = &Ways[select_way_index];

	switch (key)
	{
	case 'b':
	case 'B':
	{
		ShapeInfo* info = NewShapeInfo();
		info->way = way_info;
		info->type = S_BOX;
		info->width = info->height = 20;
		info->color = RandomColor();
		info->body = CreateBox(info);
		info->InitStartPostion();

		break;
	}
	case 'c':
	case 'C':
	{
		ShapeInfo* info = NewShapeInfo();
		info->way = way_info;
		info->type = S_CRICLE;
		info->r = 10;
		info->color = RandomColor();
		info->body = CreateCircle(info->r);
		info->InitStartPostion();
		break;
	}
	default:
		printf("key=%d\n", key);
		break;
	}

}
//构建一个有至少3个点组成的一个封闭图形
b2Body* SimulationSocialForce::CreatePolygonShape(b2Vec2 *points, int count)
{
	b2PolygonShape bodyShape;
	bodyShape.Set(points, count);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &bodyShape;
	fixtureDef.density = 0;
	fixtureDef.friction = 0;
	fixtureDef.restitution = 0;
	//碰撞层标记
	fixtureDef.filter.categoryBits = L_Player;
	//可以和哪些层碰撞
	fixtureDef.filter.maskBits = L_Player | L_Ground;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	//bodyDef.fixedRotation = true;
	bodyDef.position.Set(0, 0);
	b2Body* result = gWorld->CreateBody(&bodyDef);
	result->CreateFixture(&fixtureDef);
	return result;
}
b2Body* SimulationSocialForce::CreateBox(ShapeInfo* info)
{
	float hw = info->width*0.5f;
	float hh = info->height*0.5f;
	b2Vec2 points[] = { b2Vec2(-hw,-hh),b2Vec2(-hw,hh),b2Vec2(hw,hh),b2Vec2(hw,-hh) };
	return CreatePolygonShape(points, 4);
}
b2Body* SimulationSocialForce::CreateCircle(float r)
{
	b2CircleShape bodyShape;
	bodyShape.m_radius = r;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &bodyShape;
	fixtureDef.density = 0;
	fixtureDef.friction = 0;
	fixtureDef.restitution = 0;
	fixtureDef.filter.categoryBits = L_Player;
	fixtureDef.filter.maskBits = L_Player | L_Ground;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.position.Set(0, 0);
	b2Body* result = gWorld->CreateBody(&bodyDef);
	result->CreateFixture(&fixtureDef);
	return result;
}
