#include<iostream>
#include<vector>
#include<string>
#include<cmath>
#include<queue>
#include<stack>
#include <unistd.h>
#include <fstream>

using namespace std;


/*
Map - Vertices (Graph)
Constructing of a map
Movement to specific Point
Collision Avoidance Mechanism
Automatic Charging


1 1 1 1 1 1 1 1 1 1
1 0 0 0 0 0 0 0 0 1
1 1 1 0 1 1 1 1 0 1
1 1 1 0 1 1 1 1 0 1
1 1 1 0 1 1 1 1 0 1
1 1 1 0 1 1 1 1 0 1
1 1 1 0 0 0 0 0 0 1
1 0 0 0 0 0 0 0 0 1
1 1 1 1 1 1 1 1 1 1

1 1 1 1 1 1 1 1 1 1
1 * * * * O * * * 1
1 1 1 * 1 1 1 1 * 1
1 1 1 * 1 1 1 1 * 1
1 1 1 * 1 1 1 1 * 1
1 1 1 * 1 1 1 1 * 1
1 1 1 ? * * * * * 1
1 * * * * * * * * 1
1 1 1 1 1 1 1 1 1 1

1 1 1 1 1 1 1 1 1 1
1       O         1
1 1 1   1 1 1 1   1
1 1 1   1 1 1 1   1
1 1 1   1 1 1 1   1
1 1 1   1 1 1 1   1
1 1 1 ?           1
1                 1
1 1 1 1 1 1 1 1 1 1



*/



/*
BFS - Queue for shortest path from point A to point B
*/

/************************************************************************************************/


/* Two dimensional vector depicting the map of the arena*/
//vector<vector<int> > map; 
int x_size = 9;
int y_size = 10;
int max_x = x_size-1;
int max_y = y_size-1;
/*

Mapping Specifics

-1 => unmapped places
0  => White Line / Lane
1  => Boundary (Wall)
2 => bot
*/

/*coordinate in a map*/
struct coordinate{
	int x_cord;
	int y_cord;
};

/*charge points declaration*/
int charge_x;
int charge_y;
float bttry_min = 15;
/*
int map[9][10] = {{1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,0,1},{1,1,1,0,1,1,1,1,0,1},{1,1,1,0,1,1,1,1,0,1},{1,1,1,0,1,1,1,1,0,1},{1,1,1,0,1,1,1,1,0,1},{1,1,1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1,1,1}};
*/

/*File Input based map vector*/
vector< vector<int> > map;

void map_printing(){
	for(int i=0;i<x_size;i++){
			for(int j=0;j<y_size;j++){
				if(map[i][j]==1){
					cout<<"1 ";
				}
				else if(map[i][j]==0){
					cout<<"  ";
				}
				else if(map[i][j]==2){
					cout<<"? ";
				}
				else if(map[i][j]==5){
					cout<<". ";
				}
			}
			cout<<endl;
		}
	cout<<endl;
	return;
}

void set_map(int a, int b){
	for(int i=0;i<x_size;i++){
		for(int j=0;j<y_size;j++){
			if(map[i][j]==a){
				map[i][j]=b;
			}
		}
	}
}

class vehicle{
	
	int pos_x;
	int pos_y;
	int vel;
	int vel_max;
	int dir;
	float bttry_level;
	bool isCharging;
	bool goingToDock;
	
	/* && Instead of coordinate routes - directions can also be used && */
	queue<coordinate> route;
	
	public:
	/*constructor adding default position*/
	vehicle();
	
	//void map();
	void bfs(int x,int y);
	void bfs(int x,int y,int a,int b);
	void set_position(int x,int y);
	void motion(); //random motion
	int motion(int x, int y);
	void show_current_position();
	void show_bttry_level();
	bool charge_req();
	void chargeBattery(int, int);
	float getBtryLvl();
	void set_bttry_level(float a);
	void idleDischarge();
	int getX();
	int getY();
	
	/*setting current position*/
	void set_pos(int x, int y);
};

vehicle::vehicle() {
	pos_x = pos_y = 1;
	map[pos_x][pos_y]=2;
	bttry_level = 99;
	goingToDock = false;
}


float vehicle::getBtryLvl() {
	return bttry_level;
}

int vehicle::getX() {
	return pos_x;
}

int vehicle::getY() {
	return pos_y;
}

void vehicle::idleDischarge() {
	bttry_level -= 0.25;
}

void vehicle::show_current_position(){
	cout<<"Position: "<<pos_x<<" "<<pos_y<<endl;
}

void vehicle::show_bttry_level(){
	cout<<"Battery Level: "<<bttry_level<<endl;
}

void vehicle::set_bttry_level(float a){
	bttry_level = a;
}

void vehicle::set_position(int x,int y){
	map[pos_x][pos_y]=0;
	pos_x=x;
	pos_y=y;
	map[pos_x][pos_y]=2;
}

void vehicle::bfs(int x, int y) {
	/*empty the route queue*/
	while(!route.empty()) {
		route.pop();
	}
	
	/*using breadth first search for finding the shortest path*/
	
	/*queue for storing all bfs paths*/
	queue<coordinate> bfsq;
	
	/*parent for each coordinate*/
	vector<vector<int> > parent;
	parent.resize(x_size);
	for(int i=0;i<x_size;i++){
		parent[i].resize(y_size,0);
	}
	
	/*
	parent values - directions
	1 => right
	2 => left
	3 => up
	4 => below
	*/
	
	
	coordinate next,temp;
	next.x_cord=pos_x;
	next.y_cord=pos_y;
	bfsq.push(next);
	
	int temp_x;
	int temp_y;

	/* building bfs paths till the destination is found */
	while((next.x_cord!=x || next.y_cord!=y) && !bfsq.empty()){

		temp_x = next.x_cord;
		temp_y = next.y_cord;
		if(temp_x-1 >= 0 && (map[temp_x-1][temp_y]==0 || map[temp_x-1][temp_y]==2) && parent[temp_x-1][temp_y] == 0) {
			parent[temp_x-1][temp_y] = 1;
			temp.x_cord=temp_x-1;
			temp.y_cord=temp_y;
			bfsq.push(temp);
		}
		if(temp_x+1 <= max_x && (map[temp_x+1][temp_y]==0 || map[temp_x+1][temp_y]==2) && parent[temp_x+1][temp_y] == 0) {
			parent[temp_x+1][temp_y] = 2;
			temp.x_cord=temp_x+1;
			temp.y_cord=temp_y;
			bfsq.push(temp);
		}
		if(temp_y-1 >= 0 && (map[temp_x][temp_y-1]==0 || map[temp_x][temp_y-1]==2) && parent[temp_x][temp_y-1] == 0) {
			parent[temp_x][temp_y-1] = 4;
			temp.x_cord=temp_x;
			temp.y_cord=temp_y-1;
			bfsq.push(temp);
		}
		if(temp_y+1 <= max_y && (map[temp_x][temp_y+1]==0 || map[temp_x][temp_y+1]==2) && parent[temp_x][temp_y+1] == 0) {
			parent[temp_x][temp_y+1] = 3;
			temp.x_cord=temp_x;
			temp.y_cord=temp_y+1;
			bfsq.push(temp);
		}
		bfsq.pop();
		next=bfsq.front();
	}

	stack<coordinate> bfstck;
	/*retracing the shortest path using stack and pushing it to queue*/
	while(next.x_cord!=pos_x || next.y_cord!=pos_y){
		usleep(5000);
		bfstck.push(next);	
				
		temp_x = next.x_cord;
		temp_y = next.y_cord;
		
		if(parent[temp_x][temp_y]==1){
			next.x_cord = temp_x+1;
			next.y_cord = temp_y;
		}
		else if(parent[temp_x][temp_y]==2){
			next.x_cord = temp_x-1;
			next.y_cord = temp_y;
		}		
		else if(parent[temp_x][temp_y]==3){
			next.x_cord = temp_x;
			next.y_cord = temp_y-1;
		}
		else if(parent[temp_x][temp_y]==4){
			next.x_cord = temp_x;
			next.y_cord = temp_y+1;
		}
	}
	
	/* && Instead of coordinate routes - directions can also be used && */
	while(!bfstck.empty()){
		route.push(bfstck.top());
		bfstck.pop();
	}
	
	return;
}

void vehicle::bfs(int x, int y,int a,int b) {
	/*empty the route queue*/
	while(!route.empty()) {
		route.pop();
	}
	
	/*using breadth first search for finding the shortest path*/
	
	/*queue for storing all bfs paths*/
	queue<coordinate> bfsq;
	
	/*parent for each coordinate*/
	vector<vector<int> > parent;
	parent.resize(x_size);
	for(int i=0;i<x_size;i++){
		parent[i].resize(y_size,0);
	}	
	
	coordinate next,temp;
	next.x_cord=pos_x;
	next.y_cord=pos_y;
	bfsq.push(next);
	
	int temp_x;
	int temp_y;

	/* building bfs paths till the destination is found */
	while((next.x_cord!=x || next.y_cord!=y) && !bfsq.empty()){

		temp_x = next.x_cord;
		temp_y = next.y_cord;
		if(temp_x-1 >= 0 && (map[temp_x-1][temp_y]==0 || map[temp_x-1][temp_y]==2) && parent[temp_x-1][temp_y] == 0 && (temp_x-1!=a || temp_y!=b)) {
			parent[temp_x-1][temp_y] = 1;
			temp.x_cord=temp_x-1;
			temp.y_cord=temp_y;
			bfsq.push(temp);
		}
		if(temp_x+1 <= max_x&&(map[temp_x+1][temp_y]==0||map[temp_x+1][temp_y]==2) && parent[temp_x+1][temp_y]==0 &&(temp_x+1!=a || temp_y!=b)){
			parent[temp_x+1][temp_y] = 2;
			temp.x_cord=temp_x+1;
			temp.y_cord=temp_y;
			bfsq.push(temp);
		}
		if(temp_y-1 >= 0 && (map[temp_x][temp_y-1]==0 || map[temp_x][temp_y-1]==2) && parent[temp_x][temp_y-1] == 0&&(temp_x!=a ||temp_y-1!=b)) {
			parent[temp_x][temp_y-1] = 4;
			temp.x_cord=temp_x;
			temp.y_cord=temp_y-1;
			bfsq.push(temp);
		}
		if(temp_y+1 <= max_y&&(map[temp_x][temp_y+1]==0||map[temp_x][temp_y+1]==2) && parent[temp_x][temp_y+1] == 0&&(temp_x!=a ||temp_y+1!=b)) {
			parent[temp_x][temp_y+1] = 3;
			temp.x_cord=temp_x;
			temp.y_cord=temp_y+1;
			bfsq.push(temp);
		}
		bfsq.pop();
		next=bfsq.front();
	}

	stack<coordinate> bfstck;
	/*retracing the shortest path using stack and pushing it to queue*/
	while(next.x_cord!=pos_x || next.y_cord!=pos_y){
		usleep(5000);
		bfstck.push(next);	
				
		temp_x = next.x_cord;
		temp_y = next.y_cord;
		
		if(parent[temp_x][temp_y]==1){
			next.x_cord = temp_x+1;
			next.y_cord = temp_y;
		}
		else if(parent[temp_x][temp_y]==2){
			next.x_cord = temp_x-1;
			next.y_cord = temp_y;
		}		
		else if(parent[temp_x][temp_y]==3){
			next.x_cord = temp_x;
			next.y_cord = temp_y-1;
		}
		else if(parent[temp_x][temp_y]==4){
			next.x_cord = temp_x;
			next.y_cord = temp_y+1;
		}
	}
	
	/* && Instead of coordinate routes - directions can also be used && */
	while(!bfstck.empty()){
		route.push(bfstck.top());
		bfstck.pop();
	}
	
	return;
}

int vehicle::motion(int x, int y){
	/*checking valid destination */
	if(!(map[x][y]==0 || map[x][y]==2)){
		return -1;
	}
	show_current_position();
	show_bttry_level();
	map_printing();

	/*finding the shortest route from current(pos_x,pos_y) to destination(x,y)*/
	bfs(x,y);
	bool collision=false;
	
	coordinate next;
	bool chrge = false;
	map[pos_x][pos_y]=0;
	
	while(!collision){
		/*using the route queue make the motion*/
		while(!route.empty()) {
			next=route.front();
		
			/*assuming bots position to be marked in map with -2*/
		
			if(map[next.x_cord][next.y_cord]==2){
				//collision avoidance mechanism
				//wait for random time .. if still collision .. then alternate route otherwise skip to usual path
				collision = true;
				break;
			}

			pos_x = next.x_cord;
			pos_y = next.y_cord;
			bttry_level -= 0.5;
			if(bttry_level <= bttry_min && !goingToDock) {
				chrge = true;
				break;
			}
			route.pop();
			map[pos_x][pos_y] = 5;
			/*simulating unit travel time*/
			usleep(500);
		}
		if(collision){
			bfs(x,y,next.x_cord,next.y_cord);
			collision = false;
			continue;
		}
		if(chrge && !goingToDock) {
			chargeBattery(x, y);
		}
		collision = true;
	}
	show_current_position();
	show_bttry_level();
	map_printing();
	set_map(5,0);
	map[pos_x][pos_y]=2;
}

void vehicle::chargeBattery(int x, int y) {
	goingToDock = true;
	motion(charge_x, charge_y);
	isCharging = true;
	while(bttry_level < 100) {
		usleep(1000);
		bttry_level += 0.25;
	}
	isCharging = false;
	goingToDock = false;
	motion(x, y);
}

bool vehicle::charge_req(){
	if(bttry_level<bttry_min){
		motion(charge_x,charge_y);
	}
	else{
		bttry_level-=0.001;
	}
	
}

int main(){
	
	/*taking input of map from file generated by the bot*/
	fstream myfile("bot_data.txt", ios_base::in);
	
	myfile >> x_size >> y_size;
	myfile >> charge_x >> charge_y;
	map.resize(x_size);
	for(int i=0;i<x_size;i++){
		map[i].resize(y_size);
		for(int j=0;j<y_size;j++){
			myfile >>map[i][j];
		}
	}
	
	
	int n; //total number of vehicles
	int m; //total number of instructions
	/*creating n vehicle objects*/
	fstream instructfile("bot_instructions.txt", ios_base::in);
	instructfile >> n >> m;
	vector<vehicle> vehicles(n);
	
	int a,b,c,d;
	
	/*	
	0 x y- set position
	1 x y - motion to this point
	2 - show battery level
	3 - show current position
	4 - map_printing
	*/
	while(m>0){
		instructfile >> a >> b >> c >> d;
		if(b == 0){
			vehicles[a-1].set_position(c,d);
		}
		else if(b == 1){
			vehicles[a-1].motion(c,d);
		}
		else if(b == 2){
			vehicles[a-1].show_bttry_level();
		}
		else if(b == 3){
			vehicles[a-1].show_current_position();
		}
		else if(b == 4){
			map_printing();
		}
		else if(b==5){
			vehicles[a-1].set_bttry_level(float(c+d*0.01));
		}
		for(int i = 0; i<n; i++) {
			vehicles[a-1].idleDischarge();
			if(vehicles[a-1].getBtryLvl() < bttry_min) {
				vehicles[a-1].chargeBattery(vehicles[a-1].getX(), vehicles[a-1].getY());
			}
		}
		m--;
	}
	myfile.close();
	instructfile.close();
}
