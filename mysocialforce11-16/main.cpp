
#include "SimulationSocialForce.h"
//受力大小，每一帧都会施加

//按b暂停
bool PAUSE = false;
bool PAUSE_CREATE = false;
float CreateShapeTime = 0;

DWORD OldTickCount = 0;
GLsizei  SceneWidth = 700, SceneHeight = 500;
SimulationBase* simulation = nullptr;
std::fstream out_file;

void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}
void winReshapeFcn(int newWidth, int newHeigh)
{
	selectFont(13, ANSI_CHARSET, "Comic Sans MS");
	glLoadIdentity();
	glViewport(0, 0, SceneWidth, SceneHeight);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-1, 1, -1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
}

void DrawGLTCL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1);//设置清除颜色  
	glClear(GL_COLOR_BUFFER_BIT);
	if (simulation != nullptr)
	{
		simulation->Draw();
	}
	glFlush();
}



void UpdateFrame(float frame_time)
{
	if (!PAUSE_CREATE && (CreateShapeTime += frame_time) > .10f) //控制气泡创建的速度
	{
		if(simulation)simulation->OnKeyDown('c', 0, 0);
		CreateShapeTime = 0;
	}
	if (simulation != nullptr)
	{
		simulation->Update(frame_time);
	}
	glutPostRedisplay();
}
void MainLoop(int id)
{

	DWORD CurrentTickCount = GetTickCount();
	int dif_count = (CurrentTickCount - OldTickCount);
	int update_count = UPDATE_TIME * 1000;
	if (dif_count > update_count)
	{
		if(!PAUSE)UpdateFrame(update_count / 1000.0f);
		OldTickCount += update_count;
	}
	glutTimerFunc(MAIN_LOOP_TIME, MainLoop, id);
}

void OnKeyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	/*case 'p':
	case 'P':
	{
		PAUSE = !PAUSE;
		break;
	}*/
	case  'd':
	case  'D':
	{
		PAUSE_CREATE = !PAUSE_CREATE;
		break;
	}
	default:
	printf("key=%d\n",key);
	if (simulation)
	{
		simulation->OnKeyDown(key, x, y);
	}
	break;
	}

}
bool InitData()
{
	simulation = new SimulationSocialForce();
	simulation->m_SenceHeight = SceneHeight;
	simulation->m_SenceWidth = SceneWidth;
	simulation->Init();
	out_file.open("out_put.txt",ofstream::out);
	if (out_file.fail())
	{
		return false;
	}
	return true;
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SceneWidth, SceneHeight);
	glutCreateWindow("Scene1");
	glutDisplayFunc(DrawGLTCL);
	glutReshapeFunc(winReshapeFcn);
	OldTickCount = GetTickCount();
	if (!InitData())return 1;
	glutTimerFunc(MAIN_LOOP_TIME, MainLoop, 0);
	glutKeyboardFunc(OnKeyDown);
	glutMainLoop();
	return 0;
}