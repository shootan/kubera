#pragma once

#include "header.h"
#include "string.h"
//#include <iostream>
//using namespace std;

enum { DIRECTION_LEFT, DIRECTION_LEFTUP, DIRECTION_UP, DIRECTION_RIGHTUP,  DIRECTION_RIGHT, 
	DIRECTION_RIGHTDOWN, DIRECTION_DOWN, DIRECTION_LEFTDOWN };

//노드에 대한 정의
typedef struct node_s {
	int degree;                          //현재 이 노드의 깊이 값. 즉, 자료 구조 트리에서 깊이 값. 
	int distance;                        //이 노드로부터 목적지까지의 거리
	int value_factor;                   //평가치 값( degree + distance ) 

	int x, y;                           //이 노드의 위치 (그리드의 위치) 
	struct node_s* direct[8];       //확장 가능한 8방향에 대한 노드
	struct node_s* prev_node;    //링크드 리스트 이전 노드 
	struct node_s* next_node;     //링크드 리스트 다음 노드 

} node_t; 

//노드를 재정렬할 때 쓸 스택.
typedef struct node_stack_s {
	node_t*  node;                            //이 스택 위치에서 가리키고 있는 노드 
	struct node_stack_s*  next_node;    //이 스택의 다음 위치  
} node_stack_t;


class Astar
{
private:
	node_t *OPEN;
	node_t *CLOSED;
	node_stack_t* STACK;

	int FINDPATH_LIMIT;

public:
	Astar();
	~Astar();

	bool is_available_grid( int x, int y ); //이동 가능한지 판단하는 함수
	node_t* is_open(int x, int y); // 해당 위치의 노드가 OPEN NODE에 존재하는지 파악하는 함수
	node_t* is_closed(int x, int y); // 해당 위치의 노드가 CLOSED NODE에 존재하는지 파악하는 함수

	void push_into_stack(node_t* node); //해당 노드를 스택공간에 삽입하는 함수
	node_t* pop_from_stack(); //스택공간에서 노드를 꺼내고 해당 스택공간 삭제하는 함수

	void make_sort(node_t *old); //재평가할 노드의 기준 
	void insert_node(node_t* present);  //확장한 새 노드
	void extend_child_node(node_t* node, int x, int y, int dest_x, int dest_y, int cur_direct ); //확장할 노드, 현재 노드 위치, 목적지 위치, 확장할 방향 인덱스
	char make_child(node_t* node, int dest_x, int dest_y);  //현재 탐색할 노드 목적지 반환값은 현 노드의 확장 여부
	node_t* find_path(int start_x, int start_y, int dest_x, int dest_y); //길찾기 함수 (시작점좌표, 목적지 좌표)
};

