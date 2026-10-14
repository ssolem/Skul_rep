#include "Framework.h"

void Heap::Insert(Node* node)
{
	heap.push_back(node);
	UpdateUpper(heap.size() - 1);
}

void Heap::UpdateUpper(int index)
{
	//상향 정렬은 하향 정렬의 역개념

	int curIndex = index;
	int parent = (curIndex - 1) / 2; //홀짝 상관없이 부모 불러오기

	while (curIndex != 0) //0도달 시 종료
	{
		if (heap[parent]->GetCost() <= heap[curIndex]->GetCost())
			return; //부모와도 대소비교가 안되면 종료

		Swap(curIndex, parent);

		curIndex = parent;
		parent = (curIndex - 1) / 2;

		//UpdateLower(curIndex); //'자식끼리' 옆 비교, 상향 주 하향 정렬까지 (비추)

		//이 과정 반복으로 루트까지 갈 수 있다.
	}
}

Node* Heap::DeleteRoot()
{
	Node* root = heap.front();	//[0]
	Swap(0, heap.size() - 1);	//지금 뺀 front()를 벡터 끝으로 보내버린다.
	heap.pop_back();
	UpdateLower(0);

	return root;
}

void Heap::UpdateLower(int index)
{
	// 트리의 위에서부터 원소를 재정렬하는 과정

	int curIndex = index;	//정렬의 기준이 되어야 할 자신의 원소를 기록
	int lChild = curIndex * 2 + 1; // 하위 자식 원소의 왼쪽
	int rChild = curIndex * 2 + 2; // 하위 자식 원소의 오른쪽(lchild + 1)

	int minNode = curIndex; // 현재 최소 노드로 간주 중인 곳은 어딘가.

	while (true) //트리의 아래와 두 자식 사이를 계속 비교하면서 내려간다.
	{
		//비교 대상은 각 노드(에 도달하기까지)의 비용

		if (lChild < heap.size() &&	//lChild가 벡터안에 존재하고
			heap[lChild]->GetCost() < heap[minNode]->GetCost()) //코스트가 더 작다면
			minNode = lChild;

		if (rChild < heap.size() &&
			heap[rChild]->GetCost() < heap[minNode]->GetCost())
			minNode = rChild;

		if (minNode == curIndex) //최소 노드 인덱스가 바뀐게 없다.
			return;

		//정렬이 끝나지 않았다면, curIndex와 minNode는 서로 다른 인덱스
		Swap(curIndex, minNode); //자리 바꿈

		//바뀐 데이터에 따라 부모(기준) 자식 인덱스를 다시 지정한다.
		curIndex = minNode; //지정된 최소 노드로 재지정

		lChild = curIndex * 2 + 1;
		rChild = lChild + 1;

		//과정 반복 실행
		// 1. 자식이 위로 올라오고 재비교
		// 2. 자식끼리 비교하면서 재정렬
		// 3. 자식 비교 후 curIndex 변경
		// 4. 정렬 변화 없음 - 4번이면 종료 
	}
}

void Heap::Swap(int n1, int n2)
{
	swap(heap[n1], heap[n2]);
}
