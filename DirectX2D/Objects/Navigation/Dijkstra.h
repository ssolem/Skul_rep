#pragma once

class Dijkstra
{
public:
	Dijkstra() = default; //시작 시점엔 노드가 없음
	~Dijkstra();

	void Update();
	void Render();

	int FindClosestNode(Vector2 pos);		//지정된 위치에서(캐릭터 위치, 마우스 커서)
											// 제일 가까운 노드를 찾아주는 함수

	void SetEdgeInfo(int curIndex);			//노드를 제시하면 주변 경로로 가는 
											//진입 비용 정보를 계산하는 함수

	vector<Vector2> GetPath(int start, int end);	//노드에서 실질 경로를 찾는 함수
													//매개변수는 둘다 노드의 인덱스

	int FindMinCostNode();		//주어진 정보 중에서 가장 비용이 낮은 인접 경로 찾기

	//길찾기 앞뒤로 경로 계산 결과를 리셋하는 함수
	void ResetNodes();

	void CreateNode(Vector2 pos);

private:

private:

	vector<DNode*> nodes;				//지금 있는 노드

	DNode* selectedNode = nullptr;		//지금 보는 노드
};

