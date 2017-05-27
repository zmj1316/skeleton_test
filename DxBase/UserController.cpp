#include "UserController.h"

UserController* UserController::instance;
UserController::UserController()
{
	play = true;
	y = 0;
	QueryPerformanceCounter(&t2);

}


UserController::~UserController()
{
}

void UserController::render()
{
	// 精确计时
	count++;
	QueryPerformanceFrequency(&tf);
	QueryPerformanceCounter(&t0);
	now_time += 1.0 * (t0.QuadPart - t2.QuadPart) / tf.QuadPart;
	if (now_time >= 1.0)
	{
		QueryPerformanceFrequency(&tf);
		now_time = 0;
		fps = count;
		count = 0;
	}
	QueryPerformanceCounter(&t2);

	ImGui_ImplDX11_NewFrame();
	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiSetCond_FirstUseEver);
	static bool show_another_window = true;
	static bool show_light_window = true;

	ImGui::Begin("Control", &show_another_window);
	if (ImGui::Button("Play", ImVec2(50, 30)))
		TogglePlay();
	if (ImGui::Button("Reset", ImVec2(50, 30)))
		reset();
	if (useFABRIK)
	{
		if (ImGui::Button("FABRIK", ImVec2(50, 30)))
			ToggleIK();
	}
	else
	{
		if (ImGui::Button("CCD", ImVec2(50, 30)))
			ToggleIK();
	}
	if (vsync)
	{
		if (ImGui::Button("Vsync", ImVec2(50, 30)))
			vsync = !vsync;
	}
	else
	{
		if (ImGui::Button("No Vsync", ImVec2(50, 30)))
			vsync = !vsync;
	}
//	ImGui::SliderFloat("IK-x", &x, -5, 5);
//	ImGui::SliderFloat("IK-y", &y, -2, 5);
//	ImGui::SliderFloat("IK-z", &z, -5, 5);
	ImGui::LabelText("fps","%6d", fps);
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(100, 400), ImGuiSetCond_FirstUseEver);

	ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiSetCond_FirstUseEver);

	ImGui::Begin("Light", &show_light_window);

	ImGui::SliderFloat("Light-x", &lx, -1, 1);
	ImGui::SliderFloat("Light-y", &ly, -1, 1);
	ImGui::SliderFloat("Light-z", &lz, -1, 1);
	ImGui::End();

	ImGui::Render();
}
