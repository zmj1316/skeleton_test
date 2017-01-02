#include "UserController.h"

UserController* UserController::instance;
UserController::UserController()
{
	play = true;
	y = 0;
}


UserController::~UserController()
{
}
