#pragma once
#include "SETTINGS.h"

class UserController
{
private:
	static UserController *instance;

	LARGE_INTEGER t0, t1, t2, tf;
	double now_time = 0;
	int count = 0;
	int fps = 0;
public:
	static UserController* getInstance()
	{
		if (instance == nullptr)
			instance = new UserController();
		return instance;
	}

	bool play;
	bool useFABRIK = true;
	bool vsync = true;

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

	void render();
};
