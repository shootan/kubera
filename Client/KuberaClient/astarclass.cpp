#include "astarclass.h"

Astar::Astar(void)
{
	OPEN = NULL;
	CLOSED = NULL;
	STACK = NULL;

	FINDPATH_LIMIT = 20000;

	node_t* temp;
	node_stack_t* stack;

	while(OPEN) {
		temp = OPEN->next_node;
		free(OPEN);
		OPEN = temp;
	}

	while(CLOSED) {
		temp = CLOSED->next_node;
		free(CLOSED);
		CLOSED = temp;
	}

	while(STACK) {
		stack = STACK->next_node;
		free(STACK);
		STACK = stack;
	}
}

Astar::~Astar(void)
{

}


bool Astar::is_available_grid( int x, int y )  {  //나중에 못가는 구역 설정만 바꿔주면 됨


	if( x >= 10 && x <= 40 && y >= -15 && y <= 15)
		return 0;
	//if( x == 5 && y == 1 )
	//	return 0;
	//if( x == 5 && y == 2 )
	//	return 0;
	////if( x == 4 && y == 2 )
	////	return 0;
	//if( x == 5 && y == 4 )
	//	return 0;
	//if( x == 8 && y == 2 )
	//	return 0;
	//if( x == 8 && y == 3 )
	//	return 0;
	//if( x == 8 && y == 4 )
	//	return 0;
	//if( x == 9 && y == 5 )
	//	return 0;

	return 1;
}



node_t* Astar::is_open(int x, int y) {

	node_t *temp = OPEN;

	while(temp) {
		if(temp->x==x && temp->y==y) 
			return temp;		
		temp = temp->next_node;
	}

	return NULL;
}



node_t* Astar::is_closed(int x, int y) {

	node_t *temp = CLOSED;

	while(temp) {
		if(temp->x==x && temp->y==y) 
			return temp;
		temp = temp->next_node;
	} 

	return NULL;

} 





void Astar::push_into_stack(node_t* node) {

	node_stack_t* temp;

	temp = (node_stack_t *)calloc(1, sizeof(node_stack_t));
	temp->node= node;
	temp->next_node=STACK;
	STACK=temp;

} 



node_t* Astar::pop_from_stack() {

	node_t *temp;
	node_stack_t* stack;

	stack=STACK;
	temp=stack->node;
	STACK=stack->next_node;

	free(stack);

	return temp;

}




void Astar::make_sort(node_t *old) {

	node_t *direct, *previousNode;
	int i;
	int degree = old->degree+1;

	for(i=0; i<8; i++)	{

		if((direct=old->direct[i])==NULL) 
			continue;

		if(direct->degree > degree) {
			direct->prev_node = old;
			direct->degree	 = degree;
			direct->value_factor	 = direct->distance+direct->degree;			
			push_into_stack(direct);
		}

	}

	while(STACK) {

		previousNode = pop_from_stack();

		for(i=0; i<8; i++)	{

			if((direct=previousNode->direct[i])==NULL) 
				break;	

			if(direct->degree > previousNode->degree+1) {
				direct->prev_node = previousNode;
				direct->degree	 = previousNode->degree+1;
				direct->value_factor	 = direct->distance+direct->degree;
				push_into_stack(direct);
			}
		}
	}
}



void Astar::insert_node(node_t* present) {

	node_t* old = NULL, *temp = NULL;

	if(OPEN == NULL) {
		OPEN = present;
		return;
	} 

	temp=OPEN;

	//OPEN에 있는 노드가 현재 자식 노드보다 평가치가 낮으면
	//OPEN노드를 추적해서 현재 자식 노드보다 평가치가 높은 노드를 찾는다. 
	while(temp && (temp->value_factor < present->value_factor)) {
		old=temp;		
		temp=temp->next_node;
	} 

	//낮은 평가치의 OpenNode 중 출발지에 가장 가까운 노드 -> 추가할 자식 노드 -> 높은 평가치의 OpenNode중 출발지에 가장 먼 노드
	if(old) {
		present->next_node = temp;
		old->next_node = present;
	}else {
		present->next_node = temp;
		OPEN = present;
	} 

} 



void Astar::extend_child_node(node_t* node, int x, int y, int dest_x, int dest_y, int cur_direct ) {

	node_t* old = NULL, *child = NULL;
	int i;
	int degree= node->degree+1;

	if(old=is_open(x, y)) {

		node->direct[ cur_direct ] = old;

		//그리고 노드 재연결 및 설정 . 참고로 노드 방향이 반대로 적용된다는 걸 고려할 것. 
		if(degree<old->degree) {
			old->prev_node = node;
			old->degree = degree;
			old->value_factor = old->distance + old->degree;
		}//if()

	}else if(old=is_closed(x, y)) {

		node->direct[ cur_direct ] = old;

		//어떤 경우에는 기존 CloseNode에 있는 노드의 degree가 더 적을 경우가 발생할 수도 있는데
		//이 때 순서를 다시 설정      
		if(degree<old->degree) {
			old->prev_node = node;
			old->degree = degree;
			old->value_factor = old->distance + old->degree;
			make_sort(old);
		}//if()

		//확장할 노드 생성 및 OpenNode 리스트에 추가
	}else {
		if((child =(node_t* )calloc(1, sizeof(node_t))) == NULL)
			return;				 

		child->prev_node=node;
		child->degree = degree;
		child->distance = (x-dest_x)*(x-dest_x)+(y-dest_y)*(y-dest_y);
		child->value_factor = child->distance + child->degree;
		child->x = x;
		child->y = y;

		insert_node(child);

		node->direct[ cur_direct ] = child;

	}

}



char Astar::make_child(node_t* node, int dest_x, int dest_y) {

	int x, y;
	char flag=0;
	char checkis_available_grid[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	x=node->x;
	y=node->y;

	//이동 가능한지 여부 판단.
	checkis_available_grid[DIRECTION_RIGHTDOWN]=is_available_grid(x, y+1);
	checkis_available_grid[DIRECTION_DOWN]=is_available_grid(x, y+1);
	checkis_available_grid[DIRECTION_LEFTDOWN]=is_available_grid(x-1, y+1);
	checkis_available_grid[DIRECTION_LEFT]=is_available_grid(x-1, y);
	checkis_available_grid[DIRECTION_LEFTUP]=is_available_grid(x-1, y-1);
	checkis_available_grid[DIRECTION_UP]=is_available_grid(x, y-1);
	checkis_available_grid[DIRECTION_RIGHTUP]=is_available_grid(x+1, y-1);
	checkis_available_grid[DIRECTION_RIGHT]=is_available_grid(x+1, y);

	//각 이동 가능한 노드에 대한 자식 노드 생성 
	if(checkis_available_grid[DIRECTION_LEFT]) {
		extend_child_node(node, x-1, y, dest_x, dest_y, DIRECTION_LEFT);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_RIGHT]) {
		extend_child_node(node, x+1, y, dest_x, dest_y, DIRECTION_RIGHT);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_UP]) {
		extend_child_node(node, x, y-1, dest_x, dest_y, DIRECTION_UP);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_DOWN]) {
		extend_child_node(node, x, y+1, dest_x, dest_y, DIRECTION_DOWN);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_RIGHTDOWN] && (checkis_available_grid[DIRECTION_RIGHT] || checkis_available_grid[DIRECTION_DOWN])) {
		extend_child_node(node, x+1, y+1, dest_x, dest_y, DIRECTION_RIGHTDOWN);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_LEFTUP] && (checkis_available_grid[DIRECTION_LEFT] || checkis_available_grid[DIRECTION_UP])) {
		extend_child_node(node, x-1, y-1, dest_x, dest_y, DIRECTION_LEFTUP);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_RIGHTUP] && (checkis_available_grid[DIRECTION_RIGHT] || checkis_available_grid[DIRECTION_UP])) {
		extend_child_node(node, x+1, y-1, dest_x, dest_y, DIRECTION_RIGHTUP);
		flag=1;
	}//if()

	if(checkis_available_grid[DIRECTION_LEFTDOWN] && (checkis_available_grid[DIRECTION_LEFT] || checkis_available_grid[DIRECTION_DOWN])) {
		extend_child_node(node, x-1, y+1, dest_x, dest_y, DIRECTION_LEFTDOWN);
		flag=1;
	}//if()

	return flag;

}//char MakeChild(node_t* node, int dest_x, int dest_y)



node_t* Astar::find_path(int start_x, int start_y, int dest_x, int dest_y) {

	node_t* present, *best=NULL;
	int count=0;

	//시작 위치 노드 추가 
	present = (node_t* )calloc(1, sizeof(node_t));
	present->degree=0;
	present->distance=(dest_x-start_x)*(dest_x-start_x)+(dest_y-start_y)*(dest_y-start_y);
	present->value_factor=present->distance;
	present->x=dest_x;
	present->y=dest_y;

	OPEN=present;

	while(count< FINDPATH_LIMIT) { 

		if(OPEN==NULL) 
			return best;

		best = OPEN;					   //매 루프문을 돌면서 OpenNode의 후보로부터 탐색을 우선 시도합니다. 
		OPEN = best->next_node;     //그리고 그 다음 노드가 OpenNode의 최상위 후보노드로 지정되며 
		best->next_node = CLOSED;  //지금까지 구축된 최적의 노드를 이번에 탐색할 best노드와 연결함으로써 
		//현재까지 탐색된  최적의 길을 유지하게 됩니다. 
		CLOSED=best;						//이 best노드는 이번 루프에서 탐색 시도되므로 close노드로 들어가게 되는거죠. 


		if(best==NULL) 
			return NULL;

		//목적지 도착.
		if(best->x == start_x && best->y == start_y) 
			return best;

		if(make_child(best, start_x, start_y)==0 && count ==0) 
			return NULL;

		count++;	

	} 

	return best;

} 