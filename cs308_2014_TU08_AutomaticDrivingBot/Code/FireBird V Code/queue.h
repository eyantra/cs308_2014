#include<stdio.h>
#define MAX 20

struct node{
	int x;
	int y;
};

struct edge{
   //node current;
	int left;
	int top;
	int right;
	int bottom;
} edges[MAX][MAX];   


struct node queue_arr[MAX];
int parents[MAX][MAX][2];
int visited[MAX][MAX];
int processed[MAX][MAX];
int rear = -1;
int front = -1;

void queue_init(){

  front = -1;
  rear = -1;
  // for(i=0;i<MAX;i++){
  //  for(j=0;j<MAX;j++){
  //     edges[i][j].left=1;
  //     edges[i][j].top=1;
  //     edges[i][j].right=1;
  //     edges[i][j].bottom=1;
  //  }
  // }
  // edges[0][0].right=0;
  // edges[0][1].left=0;
  // edges[1][1].right=0;
  // edges[1][2].left=0;
  // edges[1][1].bottom=0;
  // edges[2][1].top=0;

}

void insert(struct node added_item)
{
   if ((rear == MAX - 1 && front == 0) || rear == front - 1)
   {
      //Q full
      //printf("\nSorry! Q is Full");
   }
   else
   {
      if (front == -1)
      {
         front++;
         rear++;
      }
      else if (rear == MAX - 1)
      {
         rear = 0;
      }
      else
      {
         rear++;
      }
      queue_arr[rear] = added_item;
      //Successful
      //printf("\n\nData inserted successfully");
   }   
} 

void del()
{
 if (front == -1)
 {
   //Q empty
  //printf("\n\nSorry! Q is Empty");
 }
 else
 {
  if (front == rear)
  {
   front = rear = -1;
  }
  else if (front == MAX - 1)
  {
   front = 0;
  }
  else
  {
   front++;
  }
  //Successful
  //printf("\nElement deleted Successfully");
 }
}
 
void display()
{
 int i;
 if (front == -1)
 {//Q empty
  //printf("\n\nSorry! Q is Empty");
 }
 else
 {
  //printf("\n\n:: Queue Elements are ::\n");
  if (front <= rear)
  {
   for (i = front; i <= rear; i++)
   {
    //printf("\n%d", q[i]);
   }
  }
  else
  {
   for (i = front; i < MAX; i++)
   {
    //printf("\n%d", q[i]);
   }
   for (i = 0; i <= rear; i++)
   {
    //printf("\n%d", q[i]);
   }
  }
 }
}
int empty(){
   if (front==-1)
      return 1;
   else return 0;
} 

