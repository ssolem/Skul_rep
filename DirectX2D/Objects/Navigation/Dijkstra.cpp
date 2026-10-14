#include "Framework.h"

Dijkstra::~Dijkstra()
{
	for (DNode* node : nodes) delete node;
}

void Dijkstra::Update()
{
	if (KEY_DOWN(VK_LBUTTON))
		CreateNode(mousePos);

	if (KEY_DOWN(VK_RBUTTON))
	{
		for (DNode* node : nodes)
		{
			if (node->collider->IsPointCollision(mousePos))
			{
				selectedNode = node;
				break;
			}
		}
	}

	if (KEY_UP(VK_RBUTTON))
	{
		if (selectedNode == nullptr) return;

		for (DNode* node : nodes)
		{
			if (node == selectedNode) continue;

			if (node->collider->IsPointCollision(mousePos))
			{
				//서로 선택한 노드끼리 edge정보 서로 넣어주기
				node->AddEdge(selectedNode);
				selectedNode->AddEdge(node);
				break;
			}
		}
	}

}

void Dijkstra::Render()
{
	for (DNode* node : nodes) node->Render();
}

int Dijkstra::FindClosestNode(Vector2 pos)
{
	//주어진 위치에서 제일 가까운 노드 찾기

	int index = -1;

	float minDistance = FLT_MAX;

	for (UINT i = 0; i < nodes.size(); ++i)
	{
		float distance = Distance(pos, nodes[i]->pos);

		if (minDistance > distance)
		{
			minDistance = distance;
			index = i;
		}
	}

	return index;
}

void Dijkstra::SetEdgeInfo(int curIndex)
{
	if (curIndex == -1) return; //없는 인덱스에 대한 조사 요구 시 종료

	//경로간 비용 계산

	nodes[curIndex]->isClosed = true;	//가능한 모든 경로를 다 검사하니까(했으니까)
										// -> 더 많은 조사는 불필요

	//주변 edge 정보를 모두 찾아내고
	// + 지금 이 노드까지 오기 위해 필요했던 비용과 합치기

	float cost = nodes[curIndex]->cost; //노드의 최종 비용

	for (DNode::Edge* edge : nodes[curIndex]->edges) //노드에서 뻗어가는 길들
	{
		if (cost + edge->cost < edge->node->cost) // 다음 길로 가는 비용이 더 싸면
		{
			edge->node->cost = cost + edge->cost; // 그 정보를 노드에 심어줌
			edge->node->via = curIndex;		// 그 노드를 간다면 curIndex 노드를 거친다.
		}
	}

}

vector<Vector2> Dijkstra::GetPath(int start, int end)
{
	//실제 길 찾기
	//일단 노드 정보 초기화
	ResetNodes();	//있는 길의 정보 원래대로

	int curIndex = start;	//시작 위치가 어딘지 확인
	nodes[curIndex]->cost = 0;	//비용 0에서 시작
	nodes[curIndex]->via = start; //경유지는 -1, 혹은 자기 자신

	for (DNode* node : nodes)
	{
		//지금 있는 노드 입장에서 curIndex에서 출발한다면
		//주변 노드를 입장할 때 cost가 어떻게 되는지 계산한다.
		SetEdgeInfo(curIndex);

		//시작 지점 기준으로 각 노드가 예상 가능한 비용이 아노게 된다.
		//FindMinCostNode(); //지금 노드 중에서 가장 비용이 낮은 노드가 어딘지도 알 수 있다.
							//논리적으로 가깝거나, 비용이 낮춰져 있거나
							//&& Edge가 형성이 돼 있어야 함.

		curIndex = FindMinCostNode(); //그 노드의 인덱스 값을 받아서 수정을 한다.

		//모든 노드를 순회할 때까지 반복한다.
	}

	//이렇게 반복문이 끝나면 시작 지점에서 시작했을 때
	//모든 길에 대해, 그 길(노도)로 가기 위한 비용 정보가 기록될 것이고,
	//그 중 어느 노드를 찍더라도, 그 노드를, 시작 지점으로부터, 찾아갈 경우
	//최단 경로가 어느 노드를 경유할 것인지가 via 정보로 기록될 것이다.

	//-> 이렇게 각 노드 그리고 경로 가능성을 끝까지 파헤치는 수법
	// DFS, 깊이 탐색

	//이제 완성된 각 노드별 비용 지도에서 목적지를 거꾸로 되짚으며
	// 목적지에 도달하는 최단 경로가 있었더라면 그 경유지가 무엇인가
	// 그것만 하나씩 역추적 - 백트래킹

	curIndex = end; //초점을 목적지로 변경

	vector<Vector2> path;

	while (curIndex != start)
	{
		path.push_back(nodes[curIndex]->pos); //현재 포지션 
		curIndex = nodes[curIndex]->via; //직전 경유지로 이동

		//오류 방지용
		if (curIndex == -1) break;
	}

	path.push_back(nodes[start]->pos);

	return path;
}

int Dijkstra::FindMinCostNode()
{
	int index = -1;
	float minCost = FLT_MAX;

	for (UINT i = 0; i < nodes.size(); ++i)
	{
		if (!nodes[i]->isClosed &&
			minCost > nodes[i]->cost)
		{
			minCost = nodes[i]->cost;
			index = i;
		}
	}

	return index;
}

void Dijkstra::ResetNodes()
{
	for (DNode* node : nodes)
	{
		node->isClosed = false;
		node->via = -1;
		node->cost = FLT_MAX;
	}
}

void Dijkstra::CreateNode(Vector2 pos)
{
	DNode* node = new DNode(pos);
	node->index = nodes.size();

	nodes.push_back(node);
}
