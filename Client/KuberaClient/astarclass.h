#pragma once

#include "header.h"
#include "string.h"
//#include <iostream>
//using namespace std;

enum { DIRECTION_LEFT, DIRECTION_LEFTUP, DIRECTION_UP, DIRECTION_RIGHTUP,  DIRECTION_RIGHT, 
	DIRECTION_RIGHTDOWN, DIRECTION_DOWN, DIRECTION_LEFTDOWN };

//��忡 ���� ����
typedef struct node_s {
	int degree;                          //���� �� ����� ���� ��. ��, �ڷ� ���� Ʈ������ ���� ��. 
	int distance;                        //�� ���κ��� ������������ �Ÿ�
	int value_factor;                   //��ġ ��( degree + distance ) 

	int x, y;                           //�� ����� ��ġ (�׸����� ��ġ) 
	struct node_s* direct[8];       //Ȯ�� ������ 8���⿡ ���� ���
	struct node_s* prev_node;    //��ũ�� ����Ʈ ���� ��� 
	struct node_s* next_node;     //��ũ�� ����Ʈ ���� ��� 

} node_t; 

//��带 �������� �� �� ����.
typedef struct node_stack_s {
	node_t*  node;                            //�� ���� ��ġ���� ����Ű�� �ִ� ��� 
	struct node_stack_s*  next_node;    //�� ������ ���� ��ġ  
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

	bool is_available_grid( int x, int y ); //�̵� �������� �Ǵ��ϴ� �Լ�
	node_t* is_open(int x, int y); // �ش� ��ġ�� ��尡 OPEN NODE�� �����ϴ��� �ľ��ϴ� �Լ�
	node_t* is_closed(int x, int y); // �ش� ��ġ�� ��尡 CLOSED NODE�� �����ϴ��� �ľ��ϴ� �Լ�

	void push_into_stack(node_t* node); //�ش� ��带 ���ð����� �����ϴ� �Լ�
	node_t* pop_from_stack(); //���ð������� ��带 ������ �ش� ���ð��� �����ϴ� �Լ�

	void make_sort(node_t *old); //������ ����� ���� 
	void insert_node(node_t* present);  //Ȯ���� �� ���
	void extend_child_node(node_t* node, int x, int y, int dest_x, int dest_y, int cur_direct ); //Ȯ���� ���, ���� ��� ��ġ, ������ ��ġ, Ȯ���� ���� �ε���
	char make_child(node_t* node, int dest_x, int dest_y);  //���� Ž���� ��� ������ ��ȯ���� �� ����� Ȯ�� ����
	node_t* find_path(int start_x, int start_y, int dest_x, int dest_y); //��ã�� �Լ� (��������ǥ, ������ ��ǥ)
};

