#pragma once
#include "SETTINGS.h"

class UserController
{
private:
	static UserController *instance;
public:
	static UserController* getInstance()
	{
		if (instance == nullptr)
			instance = new UserController();
		return instance;
	}

	bool play;
	bool useFABRIK = true;

	float x = 0;
	float y = 0;
	float z = 0;

	float lx = 1;
	float ly = 0;
	float lz = 1;

	UserController();
	~UserController();

	void TogglePlay()
	{
		play = !play;
	}

	void ToggleIK()
	{
		useFABRIK = !useFABRIK;
	}

	void reset()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	void render()
	{
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
		ImGui::SliderFloat("IK-x", &x, -5, 5);
		ImGui::SliderFloat("IK-y", &y, -2, 5);
		ImGui::SliderFloat("IK-z", &z, -5, 5);
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
};
