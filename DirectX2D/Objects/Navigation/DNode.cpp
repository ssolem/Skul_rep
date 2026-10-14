#include "Framework.h"

DNode::DNode(Vector2 pos) : pos(pos)
{
	collider = new CircleCollider(30);	//원을 클릭하면 노드와 상호작용 할 수 있도록
	collider->Pos() = pos; //노드가 트랜스폼이 아니라서 부모자식 관계 설정은 불가능
	collider->UpdateWorld();
}

DNode::~DNode()
{
	delete collider;

	for (Edge* edge : edges)
		delete edge;
}

void DNode::Render()
{
	//실제라기보단 입력용, 혹은 옵션용 충돌체들을 렌더하는 쪽에 가깝다.
	collider->Render();
	for (Edge* edge : edges)
		edge->line->Render();
}

void DNode::AddEdge(DNode* node)
{
	//노드 정보를 받으면 서로 향하는 엣지가 있다고 설정만 해둬도 된다.
	Edge* edge = new Edge(node->pos, pos);
	edge->node = node; //목적지가 어디냐가 중요

	//---------------------------------------------------------------------
	//코스트에 대해 기본 설정을 할 거라면
	//여기서 가중치를 주거나, 추가 연산, 추가 매개변수 등을 설정할 수 있다.
	// 
	// 여기서는 샘플로 기하학적 거리가 있는 그대로의 비용이 맞다고 가정
	edge->cost = Distance(node->pos, pos);

	//---------------------------------------------------------------------

	edges.push_back(edge);

}
