#include<stdio.h>

struct node stack_arr[MAX*MAX];
int top = -1;

void stack_init(){
	int i,j;
	top = -1;
	for(i=0;i<MAX;i++){
		for(j=0;j<MAX;j++){
			edges[i][j].left=0;
			edges[i][j].top=0;
			edges[i][j].right=0;
			edges[i][j].bottom=0;
		}
	}
}

void sinsert(struct node added_item) {
	if (top == MAX) {
		//printf("\nSorry! Stack is Full");
	} else {
		top++;
		stack_arr[top] = added_item;
	}
}

void sdel() {
	if (top == -1) {
		//printf("\n\nSorry! Stack is Empty");
	} else {
		top--;
	}
}

struct node spop() {
    struct node temp;
    if (top == -1) {
		//printf("\n\nSorry! Stack is Empty");
	} else {
		temp = stack_arr[top];
		top--;
	}
	return temp;
}

struct node stop() {
    if(top == -1) {
        //printf("\n\nSorry! Stack is Empty");
    }
    return stack_arr[top];
}

int sempty(){
	if (top==-1)
		return 1;
	else return 0;
}

int ssize() {
	return top+1;
}
