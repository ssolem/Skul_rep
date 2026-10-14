#include "Framework.h"
#include "Objects/Tile/GameTileMap.h"

AStar::AStar()
{
	heap = new Heap();
}

AStar::~AStar()
{
	delete heap;
	for (Node* node : nodes) delete node;
}

void AStar::ImportNodes(GameTileMap* map)
{
	map->GetNodes(nodes);	//지도에 노드 벡터를 주고, 노등 정보를 받아오기
	SetEdge(map->GetWidth());
}

void AStar::Update()
{

}

void AStar::Render()
{
	for (Node* node : nodes) node->Render();
}

int AStar::FindClosestNode(Vector2 pos)
{
	//주어진 위치에서 제일 가까운 노드 찾기

	int index = -1;

	float minDist = FLT_MAX;

	for (UINT i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i]->state == Node::State::OBSTACLE)
			continue;

		float distance = Distance(pos, nodes[i]->pos);

		if (minDist > distance)
		{
			minDist = distance;
			index = i;
		}
	}

	return index;
}

int AStar::FindRandomNodeInRange(Vector2 pos, float range)
{
	vector<Node*> targetNodes; //찾으려고 하는 노드의 벡터

	for (Node* node : nodes)
	{
		float distance = Distance(pos, node->pos);

		//갱신 대신 벡터 포함 여부만
		if (distance < range)
		{
			targetNodes.push_back(node); // 맞으면 1회 연산
		}
	}

	int randomIndex = Random(0, targetNodes.size());

	return targetNodes[randomIndex]->index; //대상 벡터의 랜덤 인덱스 노드의 자체 인덱스 반환
}

void AStar::GetPath(IN int start, IN int end, OUT vector<Vector2>& path)
{
	Reset();
	path.clear();	//초기화

	//시작 노드에 대한 정보 초기화

	float G = 0;	//진행한 경로가 없으므로, 총비용은 0
	float H = GetDiagonalManhattanDistance(start, end);	//장애물이 없었다면 걸릴거리
	//float H = GetManhattanDistance(start, end);	//대각선 이동이 불가할 경우

	//초기화된 정보를 시작 지점 노드에 입력
	nodes[start]->g = G;
	nodes[start]->h = H;
	nodes[start]->f = G + H;
	nodes[start]->via = start;
	nodes[start]->state = Node::State::OPEN;	//탐색을 시작한 상태

	//이 노드를 힙에 최초로 추가
	heap->Insert(nodes[start]);

	while (nodes[end]->state != Node::State::CLOSED) //목적지 노드의 주변 경로 탐색이 끝날 때까지
	{
		//목적지에 대한 경로 탐색 계속 반복

		if (heap->Empty()) return; //지도가 잘못되어서 노드가 하나도 안 들어간 경우
									// 길이 완전히 끊겨서, 넣어둔 힙의 정렬 자료를 싹다 뽑아 쓴 경우

		//현재 찾아낸 주변부 노드 + 자기 자신 중에서 가장 비용이 적은 노드를 찾기
		int curIndex = GetMinNode();	//힙에서 제일 빠른 노드 찾기
		Extend(curIndex, end);	//현재 위치에서 주변을 검색하고, 엣지 갱신 진행
								// 힙도 같이 갱신

		//주변 노드에 대한 경로 탐색을 마친 후 자기 자신은 종료로 기록
		nodes[curIndex]->state = Node::State::CLOSED;

	}
	// 이 반복문을 끝내면 Extend()를 제외하고 봤을 때,
	// start에서 end까지 경로로 선택할 수 있는 경우의 수 : 선택지의 집합이 나오게 된다.
	// -> 이 선택지에서 Extend()로 새 선택지를 뽑는 동안에 최소 비용 계산까지도 진행시켜 준다면
	// -> 선택지에서 , 다시 최선의 경로를 뽑아놓은 Using, 혹은 별도 데이터 집합이 나올 것

	//경로가 다 만들어졌다고 치면 : 데익스트라 때와 할 일은 같음
	//역추적 시작
	int curIndex = end;

	while (curIndex != start)
	{
		nodes[curIndex]->state = Node::State::USING;	//노드 상태를 경로 사용으로 확정
		path.push_back(nodes[curIndex]->pos);			//벡터 노드에 새 노드 위치 추가
		curIndex = nodes[curIndex]->via;
	}

	path.push_back(nodes[start]->pos);		//마지막으로 시작점 위치 추가

}

void AStar::Reset()
{
	for (Node* node : nodes)
	{
		//노드 상태 리셋
		if (node->state != Node::State::OBSTACLE) //장애물만 아니면
		{
			node->SetState(Node::State::NONE);
		}

		heap->Clear();
	}
}

float AStar::GetManhattanDistance(int start, int end)
{
	Vector2 startPos = nodes[start]->pos;
	Vector2 endPos = nodes[end]->pos;

	Vector2 d = endPos - startPos; 
	float md = abs(d.x) + abs(d.y);

	return md;
}

float AStar::GetDiagonalManhattanDistance(int start, int end)
{
	Vector2 startPos = nodes[start]->pos;
	Vector2 endPos = nodes[end]->pos;

	Vector2 d = endPos - startPos;

	//x,y 중 어느쪽이 긴지 확인한다. - 대각선 구하기 위해서
	float x = abs(d.x);
	float y = abs(d.y);

	float maxSize = max(x, y);
	float minSize = min(x, y);

	return (maxSize - minSize) + sqrt(minSize * minSize * 2); //차이 + 대각직선
}

void AStar::Extend(int center, int end)
{
	// 노드 하나 조사 중, 주변 노드(최대8개)를 돌아가면서 목적지까지의 비용을 계산하는 함수
	// -> 이 함수를 통해 주변 노드의 힙 입력과 더불어, 비용 계산을 통해 최적 경로 가능성까지 계산

	for (Node::Edge* edge : nodes[center]->edges)
	{
		int index = edge->index;

		if (nodes[index]->state == Node::State::CLOSED || nodes[index]->state == Node::State::OBSTACLE)
			continue;

		//현재까지의 경로 진행을 참작해서 경로의 비용 계산 진행

		float G = nodes[center]->g + edge->cost; //여기로 오는데까지의 비용
		float H = GetDiagonalManhattanDistance(index, end); //여기서부터의 예상 비용
		float F = G + H;

		if (nodes[index]->state == Node::State::OPEN) //이미 이곳을 보고 있는 경우면
		{
			//F 비교 후 갱신만 진행

			if (F < nodes[index]->f)
			{
				nodes[index]->g = G;
				nodes[index]->f = F;
				//nodes[index]->h 여러번 반복할 필요는 없음
				nodes[index]->via = center;
			}
		}
		else if (nodes[index]->state == Node::State::NONE) //막 처음 봤으면
		{
			nodes[index]->g = G;
			nodes[index]->h = H;
			nodes[index]->f = F;
			nodes[index]->via = center;
			nodes[index]->state = Node::State::OPEN;

			heap->Insert(nodes[index]); //힙의 노드 정보에 새 노드 추가
		}

		//이 모든 과정을 최대 8번 주변 모든곳에 반복해 주면 함수 종료
	}
}

int AStar::GetMinNode()
{
	return heap->DeleteRoot()->index;
}

void AStar::SetEdge(UINT width)
{
	for (UINT i = 0; i < nodes.size(); ++i)
	{
		if (i % width != width - 1) //맨 오른쪽칸 빼고
		{
			nodes[i]->AddEdge(nodes[i + 1]);
			nodes[i + 1]->AddEdge(nodes[i]);
		}

		if(i < nodes.size() - width) // 맨 아래칸 빼고
		{
			nodes[i]->AddEdge(nodes[i + width]);
			nodes[i + width]->AddEdge(nodes[i]);
		}

		if (i < nodes.size() - width && i % width != width - 1) //우하향
		{
			nodes[i]->AddEdge(nodes[i + width + 1]);
			nodes[i + width + 1]->AddEdge(nodes[i]);

		}

		if (i < nodes.size() - width && i % width != 0) //좌하향
		{
			nodes[i]->AddEdge(nodes[i + width - 1]);
			nodes[i + width - 1]->AddEdge(nodes[i]);
		}
	}
}