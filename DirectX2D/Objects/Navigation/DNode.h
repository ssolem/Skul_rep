#pragma once

class DNode		//도식화된 경로의 데이터
{
private:
	//부속 정보

	friend class Dijkstra; //길찾기 클래스와 데이터 공유가 잘 되도록

	struct Edge				//경로와 경로 사이의 중간 정보를 기록한 것 (간선 정보)
	{
		DNode* node;		//어느 경로로 가는가
		float cost;			//경로 선택에 드는 비용

		LineCollider* line;	//(옵션) 길 사이의 그래픽 표시에도 좋고
							//		길에서 못 이탈하게 막는 식으로 사용도 가능

		Edge(Vector2 start, Vector2 end)
		{
			line = new LineCollider(start, end);
		}

		~Edge() { delete line; }
	};

public:
	DNode(Vector2 pos);
	~DNode();

	void Render();
	void AddEdge(DNode* node);	//다른 노드와 연결하기

private:

	//데익스트로 알고리즘의 특징 : 노드의 정해진 위치가 따로 없다.

	Vector2 pos;				// 그래서 위치 설정이 자유

	bool isClosed = false;		// 길찾기 중에 현재 감안할 선택지가 더 남아 있는지 여부

	int index = 0;				// 만들어진 노드 사이에서는 몇 번으로 지칭되는가
								// 노드끼리에서의 id

	int via = -1;				// 이 노드가 경유지라고 했을 때, 
								// 어디에서 이 노드로 왔다고 계산할 것이가를 기록한 것
								// '-1' : 전혀 없음을 뜻하는 지칭
								// C ,C++ 배열에 쓰면 터뜨리는 인덱스로도 사용 (C#, Java 는 아님)

	float cost = FLT_MAX;		// 이 노드로 오는 경로가 있다고 할 때,
								// 여기 오기까지 필요했던 총 비용

	vector<Edge*> edges;		// 주변에 인접한 노드(서로 왕래 가능한)가 있다면
								// 그 노드로 오갈 때 생기는 간선 정보를 기록한 것

	Collider* collider;			// 노드를 클릭하는데 필요한 충돌체 (입력용)
};

