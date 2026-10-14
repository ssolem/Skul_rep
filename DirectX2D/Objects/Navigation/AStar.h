#pragma once

class AStar
{
public:

	AStar();
	~AStar();

	void ImportNodes(class GameTileMap* map);

	void Update();
	void Render();

	int FindClosestNode(Vector2 pos);
	int FindRandomNodeInRange(Vector2 pos, float range);

	void GetPath(IN int start, IN int end, OUT vector<Vector2>& path);

	// IN, OUT -> 윈도우에서 지정해둔 정의 매크로
	// 정의 매크로 : 텍스트로 치환하지 않고, 그런 define이 존재한다.
	//				텍스트 치환에 의한 연산에 쓸 순 없지만, 컴파일러 조건식에 제시할 수 있거나
	//				순수하게 주석처럼 소통에 활용 가능

private:

	void Reset();	// 길 찾기 할 때마다 이전 기록 없애기

	//길 찾기의 기준이 도리 휴리스틱 구하기
	float GetManhattanDistance(int start, int end);
	float GetDiagonalManhattanDistance(int start, int end);

	//지오메트리를 구한 후 다음 노드 찾기
	void Extend(int center, int end);
	int GetMinNode();

	void SetEdge(UINT width);

private:

	vector<Node*> nodes; //노드 정보들
	Heap* heap;			//정렬 기능

};

