#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct Node {
  char type;        // 'R' = room, 'J' = junction, 'E' = exit
  int id;
};

struct Edge {
  int from;
  int to;
  int steps;
  char turn; // 'L', 'R', 'S'
};

Node nodes[20];
Edge edges[50];
int nodeCount = 0;
int edgeCount = 0;
int exitNode = -1;
int startNode = -1;

// OLED helper
void showOLED(String msg){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,20);
  display.println(msg);
  display.display();
}

// Add a node
int addNode(char type){
  nodes[nodeCount].id = nodeCount;
  nodes[nodeCount].type = type;
  if(type=='R') startNode = nodeCount;
  if(type=='E') exitNode = nodeCount;
  nodeCount++;
  Serial.print("Node added: "); Serial.println(type);
  return nodeCount-1;
}

// Add edge
void addEdge(int from, int to, int steps, char turn){
  edges[edgeCount++] = {from, to, steps, turn};
}

// Dijkstra's Algorithm
const int INF = 9999;
int dist[20], prevNode[20];
bool visited[20];

void dijkstra(int n, int start){
  for(int i=0;i<n;i++){
    dist[i]=INF;
    visited[i]=false;
    prevNode[i]=-1;
  }
  dist[start]=0;

  for(int i=0;i<n;i++){
    int u=-1;
    for(int j=0;j<n;j++){
      if(!visited[j] && (u==-1 || dist[j]<dist[u])) u=j;
    }
    if(dist[u]==INF) break;
    visited[u]=true;

    for(int e=0;e<edgeCount;e++){
      if(edges[e].from==u){
        int v=edges[e].to;
        int w=edges[e].steps;
        if(dist[u]+w<dist[v]){
          dist[v]=dist[u]+w;
          prevNode[v]=u;
        }
      }
    }
  }
}

// Reconstruct path
int path[20];
int pathLen=0;

void getPath(int end){
  pathLen=0;
  for(int v=end;v!=-1;v=prevNode[v]){
    path[pathLen++]=v;
  }
  // reverse path
  for(int i=0;i<pathLen/2;i++){
    int tmp=path[i]; path[i]=path[pathLen-1-i]; path[pathLen-1-i]=tmp;
  }
}

// Print escape instructions
void printInstructions(){
  for(int i=0;i<pathLen-1;i++){
    int u=path[i];
    int v=path[i+1];
    // find edge from u->v
    int steps=0; char turn='S';
    for(int e=0;e<edgeCount;e++){
      if(edges[e].from==u && edges[e].to==v){
        steps=edges[e].steps;
        turn=edges[e].turn;
        break;
      }
    }
    showOLED("Walk "+String(steps)+" steps");
    delay(2000);
    // Reverse turn for escape
    if(turn=='L') turn='R';
    else if(turn=='R') turn='L';
    if(turn!='S'){
      showOLED("Turn "+String(turn=='L'?"Left":"Right"));
      delay(2000);
    }
  }
  showOLED("EXIT Reached!");
}

void setup(){
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
    Serial.println("OLED fail"); while(1);
  }
  showOLED("Fire Escape Ready");
  Serial.println("Commands: R=Room, J=Junction, E=Exit, N=New Node, F=Fire");
}

int lastNode=-1;

void loop(){
  if(Serial.available()){
    char cmd=Serial.read();
    if(cmd=='R'||cmd=='J'||cmd=='E'){
      int newNode=addNode(cmd);
      if(lastNode!=-1){
        Serial.println("Steps from last node?");
        while(!Serial.available());
        int steps=Serial.parseInt();
        Serial.println("Turn at last node? (L/R/S)");
        while(!Serial.available());
        char turn=Serial.read();
        if(turn!='L' && turn!='R') turn='S';
        addEdge(lastNode,newNode,steps,turn);
        addEdge(newNode,lastNode,steps,turn); // bidirectional
      }
      lastNode=newNode;
    }
    if(cmd=='F'){
      if(startNode==-1||exitNode==-1){
        showOLED("Set Room and Exit!");
      } else {
        dijkstra(nodeCount,startNode);
        getPath(exitNode);
        printInstructions();
      }
    }
  }
}
