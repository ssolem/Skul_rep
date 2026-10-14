#pragma once

//힙 : 노드 자료를 받았을 때 이진 트리 방식으로 정렬해서
//		빠르게 최소 노드를 끄집어내고 최소 노드 순서로, 역시 빠르게 빼내기 위해 만든
//		FIFO 개념 흉내낸 클래스. 메모리 '힙'을 따서 이름을 지었다.

class Heap
{
public:
	Heap() = default;
	~Heap() = default; //Node* - 참조일 뿐 객체는 다른 곳에 있을 예정

	//노드 정보를 받아 정렬하기 위한 함수들
	void Insert(Node* node);	//노드를 구조에 추가
	void UpdateUpper(int index);	//추가 시점에서 지정된 노드 위의 모든 노드 위계를 재정렬
									// -> 일부러 추가 정렬을 호출하지 않아도 트리 맨 위에
									//    최소 노드가 있도록 유지할 수 있다.

	Node* DeleteRoot();			// 큐나 힙의 'pop' 'top'에 대응하는 함수
								// -> 트리의 최소 노드를 구함과 동시에 트리에서 제거
	void UpdateLower(int index);	//제거 시점에서 지정된 노드 아래의 위계 재정렬
									// -> 이 함수를[0]에서 호출하면 -> 전체 정렬이 일어난다.

	void Swap(int n1, int n2);	//std::swap의 인덱스 버전

	void Clear() { heap.clear(); }
	bool Empty() { return heap.empty(); }

private:
	vector<Node*> heap;	//중간 정렬을 계속 진행할 예정이라
						//실제 큐, 실제 힙을 쓰진 않는다.
};

