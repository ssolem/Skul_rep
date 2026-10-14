#include "Framework.h"

Node::Node(Vector2 pos, int index) : pos(pos), index(index)
{
	collider = new CircleCollider(10);	
	collider->Pos() = pos;
	collider->UpdateWorld();
}

Node::~Node()
{
	for (Edge* edge : edges) delete edge;
	delete collider;
}

void Node::Render()
{
	switch (state)	//노드 상태에 따라 충돌체 색깔
	{
	case Node::State::NONE:	collider->GetColor() = { 0, 1, 1, 1 }; //청록
		break;
	case Node::State::OPEN:	collider->GetColor() = { 0, 0, 1, 1 }; //파
		break;
	case Node::State::CLOSED: collider->GetColor() = { 0, 0, 0, 1 }; //검
		break;
	case Node::State::USING: collider->GetColor() = { 0, 1, 0, 1 }; //초
		break;
	case Node::State::OBSTACLE: collider->GetColor() = { 1, 0, 0, 1 }; //빨
		break;
	}

	collider->Render();
	for (Edge* edge : edges)
		edge->line->Render();
}

void Node::AddEdge(Node* node)
{	
	Edge* edge = new Edge(node->pos, pos);
	edge->index = node->index;

	edge->cost = Distance(node->pos, pos); //지도상 거리가 중요 -> 휴리스틱 계산에 사용

	edges.push_back(edge);
}
