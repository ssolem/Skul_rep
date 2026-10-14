#pragma once

#include "ObjectController.h"

// (예비용) 컨트롤러가 여러 개 나오는 경우라면 작성

class GameController : public Singleton<GameController>
{
public:


private:
	vector<ObjectController> controllers;

};

