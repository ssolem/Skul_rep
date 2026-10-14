#pragma once

class Node
{
	//길찾기 알고리즘의 '길' : A*에서는 타일 1장으로 대변되는 구역 1개

public:
	friend class AStar;  //길찾기 클래스와 데이터 공유가 잘 되도록

	enum class State	//지금 노드의 상태
	{
		//계산 중에 노드가 어떤 대상으로 분류 중인지 구분하기 위함
		NONE, OPEN, CLOSED, USING, OBSTACLE
		//왼쪽부터 3개 : 대상아님, 연산중(대기중), 연산종료
		//Using : Open을 연산에 사용할 경우, 경로로 확정되어 길로 쓰이는 곳
		//		  Open을 대기에 사용할 경우, 연산의 대상으로 선별되었다는 뜻
		//Obstacle : 장애물 - 처음부터 갈 수 없다고 확정이 된 곳
	};

	struct Edge	//여기서는 옆 타일과의 관계
	{
		int index; //어디로 갈 건가
		float cost;

		LineCollider* line;

		Edge(Vector2 start, Vector2 end)
		{
			line = new LineCollider(start, end);
			line->GetColor() = { 1, 1, 0, 1 };
		}

		~Edge() { delete line; }
	};
	
public:
	
	Node(Vector2 pos, int index);	//맵이 있는 노드이므로 index가 처음부터 있으면 좋음
	~Node();

	void Render();

	void AddEdge(Node* node);

	void SetState(State state) { this->state = state; }
	float GetCost() { return f; }

private:

	State state = State::NONE;	//시작시 사용없음 초기화

	Vector2 pos;
	int index = 0;
	int via = -1;

	vector<Edge*> edges;
	Collider* collider;	//상호작용용 옵션, 시각화용

	//A* 경로 비용은 3가지로 나뉘어 기록된다.
	float f = 0;		// 최종 비용 = 지오메트리 + 휴리스틱
	float g = 0;		// 지오메트리, 혹은 형상 경로 : 확정된 경로에 의한 비용
	float h = 0;		// 휴리스틱 경로, 위치에서 목적지까지의 방향을 계산하면서 추정하는 예상 비용
	
};

