#pragma once

class NinjaIdle : public Action
{
public:
	NinjaIdle();
	~NinjaIdle() = default;

	// 가만히 있으면 그게 '아이들' : 일단 상속은 받되 나머지는 일단 대기
};

