#include "XTest.h"
#include "DirectInputClass.h"
MYAPP *g_app = NULL;
DirectInput* gDInput = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	void* p = _aligned_malloc(sizeof(XTest),16);

	g_app = new(p) XTest(L"CG_HW2", 800, 600, hInstance, nCmdShow);
	gDInput = new DirectInput(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	return g_app->Run();
}