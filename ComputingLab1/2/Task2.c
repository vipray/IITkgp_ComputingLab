/*
Name:            Vipray Jain
Assignment No. : 2
Task :           2
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>
#include<string.h>
#define max(X, Y) (((X) < (Y)) ? (Y) : (X))

//Queue used in BFS traversal
struct QueueNode{
    int val; 
    int depth;
};

struct Queue{
    int front,end; 
    int type;   //0 user, 1 content
    struct QueueNode* arr; 
}; 

struct pair{
    int idx;
    int val;
};

//Graph
struct Node{
    int dest; 
    int type;   //0 user, 1 content
    struct Node* next; 
}; 
  
struct List {
    struct Node *head;  
}; 
  
struct Graph { 
    int V ; 
    struct List* array; 
};


/*  
    deallocateGraph() - to dellocate memory of graph
    g   -   instance of graph
    
*/

void deallocateGraph(struct Graph* g){
    int V = g->V;
	for(int i=0;i<V;i++){
        struct Node* tmp1 = g->array[i].head;
        if(tmp1!=NULL){
            struct Node* tmp2 = tmp1->next;
            while(tmp2!=NULL){
                free(tmp1);
                tmp1 = tmp2;
                tmp2 = tmp2->next;
            }
		    free(tmp1);
        }
    }
	free(g->array);
	free(g);    
}
/*  
    deallocateQueue() - to dellocate memory of Queue
    g   -   instance of graph
    
*/

void deallocateQueue(struct Queue* q){
	free(q->arr);
	free(q);    
}



/*  
    intiallizeGraph(v) - funtion to intiallize Graph
*/
struct Graph* intiallizeGraph(int V){
	struct Graph* g =  (struct Graph*)(malloc(sizeof(struct Graph)));
	g->V = V;
	
	g->array = (struct List*)(malloc(V * sizeof(struct List)));	//allot V AdjList size into array
	
	int i=0;
	for(i=0;i<V;i++){
		g->array[i].head = NULL;	//Intiallize head of each node by NULL
	}
	
	return g;
}

/*  
    intiallizeQueue(v) - funtion to intiallize Queue
*/
struct Queue* intiallizeQueue(int V){
	struct Queue* q =  (struct Queue*)(malloc(sizeof(struct Queue)));
	
	q->arr = (struct QueueNode*)(malloc(V * sizeof(struct QueueNode)));
    q->front = -1;
    q->end = -1;
	
	return q;
}

/*  
    addEdge() - funtion to add edge in graph
    g   -   instant of graph
    s   -   source
    e   -   destination
    type    -   type of node (user or content)

*/
void addEdge(struct Graph* g, int s, int e, int type){
	//s->e
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->dest = e;
	newNode->type = type;

	//add node in AdjList of node 's' (in front)
	newNode->next = g->array[s].head;
	g->array[s].head = newNode;

}


/*  
    isEmpty() - funtion to check whether queue is empty or not
    q   -   instant of Queue

*/
bool isEmpty(struct Queue *q){

    return q->front == -1;
}

/*  
    enqueue() - funtion to insert in queue
    q   -   instant of Queue
    v   -   value to insert
    d   -   depth of value in BF Tree

*/
void enqueue(struct Queue *q, int v, int d){
    if(isEmpty(q)){
        q->front=0;
        q->end=0;
    }
    else{
        q->end++;
    }
    q->arr[q->end].val=v;
    q->arr[q->end].depth=d;
    
}

/*  
    dequeue() - funtion to insert in queue
    q   -   instant of Queue

*/
struct QueueNode dequeue(struct Queue *q){
    struct QueueNode qn = q->arr[q->front];
    
    if(q->front == q->end){
        q->front = -1;
        q->end = -1;
    }
    else{
        q->front++;
    }
    return qn;
}

/*  
    distanceLContentNodes() - funtion to run BFS and find top k min distance content from given src
    
    q   -   instant of Queue
    src -   user from which k min distance content is needed
    dist    -   struct array to store content node and its distance
    k   -   given parameter(limit of max number of content to reccomond )
    userCount    -   used to ensure no closed user node is further visited
    contentCount    -   used to ensure no closed content node is further visited
    distLen  -   length of dist array

*/

void distanceLContentNodes(struct Graph* g, int src, struct pair dist[], int k, int userCount, int contentCount, int *distLen)
{
    struct Queue *q = intiallizeQueue(userCount);

    bool visitedUser[userCount];
    bool visitedContent[contentCount];

    for (int i = 0; i < userCount; i++){
        visitedUser[i] = false;
    }

    for (int i = 0; i < contentCount; i++){
        visitedContent[i] = false;
    }

    visitedUser[src]=1;
    enqueue(q,src,0);
    
    
    int nextDepth=0;
    // Find shortest path for all vertices
    while(!isEmpty(q))
    {
        int type;
        struct QueueNode qn = dequeue(q);
        int u = qn.val;
        nextDepth = qn.depth + 1;
        
        visitedUser[u] = true;
        
        struct Node* tmp = g->array[u].head;
        int v,t;
        bool flag=0;
        //check all the neighbour of u
        while(tmp!=NULL){
            v = tmp->dest;
            t = tmp->type; 
            
            if(t==1 && !visitedContent[v]){
                visitedContent[v]=1;
                if(nextDepth>k){
                    flag=1;
                    break;
                }
                dist[(*distLen)].idx = v;    //content no
                dist[(*distLen)++].val = nextDepth; //distance of content    
            }
            else if(t==0 && !visitedUser[v]){
                visitedUser[v]=1;
                enqueue(q,v,nextDepth);
            }
        
            tmp = tmp->next;
        }
        if(flag){
            break;
        }
    }
    //free memory
    deallocateQueue(q);
}

/*  
    connectedUserNodes() - funtion to find top k-1 min distance user from given src user
    
    q   -   instant of Queue
    src -   user from which k min distance content is needed
    userInRace    -   users at k-1 distance from src
    k   -   given parameter(limit of max number of content to reccomond )
    maxNodes    -   used to ensure no closed user node is further visited

*/
void connectedUserNodes(struct Graph* g, int src, bool userInRace[], int k, int maxNodes)
{
    struct Queue *q = intiallizeQueue(maxNodes);
    bool visited[maxNodes];

    //initiallize
    for (int i = 0; i < maxNodes; i++){
        visited[i] = false;
    }
    
    visited[src]=1;
    userInRace[src]=1;
    enqueue(q,src,0);
     
    int nextDepth=0;
    while(!isEmpty(q))
    {
        struct QueueNode qn = dequeue(q);
        int u = qn.val;
        nextDepth = qn.depth + 1;
        
        visited[u] = true;
        
        struct Node* tmp = g->array[u].head;
        int v,t;
        bool flag=0;
        while(tmp!=NULL){
            v = tmp->dest;
            t = tmp->type; 
            if(!visited[v] && t==0){
                
                if(nextDepth>k){
                    flag=1;
                    break;
                }
                //mark user as it is in k-1 neighnour
                userInRace[v] = 1;
                visited[v]=1;
                enqueue(q,v,nextDepth);
            }
            tmp = tmp->next;
        }
        //break when done finding k-1 neighbour users
        if(flag){
            break;
        }
    }
    //free memory
    deallocateQueue(q);
}


/*  
    isEmptyLine() - funtion to check whther line is empty or not
    input   -   string

*/
int isEmptyLine(const char *input)
{
    char ch;
    int len = strlen(input);
    //int i=0;
    while (len--)
    {
        ch = *(input++);
        if(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\0')
            return 0;
    }

    return 1;
}

/*  
    isUselessChar() - funtion to neglact char of no use
    ch   -   character

*/
int isUselessChar(char ch)
{
    if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\0'){
        return 1;
    }
    return 0;
}

/*  
    parseInput() - funtion to parse file into graph
    fptr   -   file pointer
    g   -   instance of graph
    reversedGraph   -   instance of graph(from content to user)
    uc  -   pointer to maintain of userCount
    cc  -   pointer to maintain of contentCount

*/

struct Graph* parseInput(FILE *fptr, struct Graph* graph, struct Graph** reversedGraph, int *uc, int *cc){
    char input[8005];   // one line of file at a time
    
    while(fgets (input, 8000, fptr)!=NULL){
        if(isEmptyLine(input)){
            continue;
        }
        
        int len = strlen(input);
        for(int i=0;i<len;i++){
            if(isUselessChar(input[i])){
                continue;
            }
            int num=0;
            while(i<len){
                if(isUselessChar(input[i])){
                    i++;
                    continue;
                }
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            *uc = num;
        }
    
        break;  
    }
    
    graph = intiallizeGraph(*uc);
	

    int type=0;
    int co=0;
    while(co < (*uc) && (fgets (input, 8000, fptr)!=NULL)){
        if(isEmptyLine(input)){//to deal with empty line
            continue;
        }
        int len = strlen(input);
        int qw=0;
        int source, destination;
        int flag=0;
        for(int i=0;i<len;i++){
            if(input[i]==',' || isUselessChar(input[i])){
                continue;
            }
    
            int num=0;
            if(!flag){
                flag=1;
                while(i<len && input[i]!=':'){
                    if(isUselessChar(input[i])){
                        i++;
                        continue;
                    }
                    num = num*10 + ((int)(input[i]-'0'));
                    i++;
                }
                i++;
                source = num;
            }
            num=0;
            bool isthere=0;
            while(i<len && input[i]!=','){
                if(isUselessChar(input[i])){
                        i++;
                        continue;
                }
                isthere=1;
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            destination = num;
            if(isthere)
                addEdge(graph, source, destination, type);
        }
        co++;     
    }

    //for content
    while(fgets (input, 8000, fptr)!=NULL){
        if(isEmptyLine(input)){
            continue;
        }
        int len = strlen(input);
        for(int i=0;i<len;i++){
            if(isUselessChar(input[i])){
                continue;
            }
            int num=0;
            while(i<len){
                if(isUselessChar(input[i])){
                    i++;
                    continue;
                }
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            *cc = num;
        }
        break; 
    }
    
    // reveres graph is a virtual graph
    // I made it to decrease time complexity
    // It will have edges from Content to the user(hypothetical)
    // These edges will give publisher user of all content in O(1)
    // If we dont keep this, we have to search from all users to the contents
    *reversedGraph = intiallizeGraph(*cc);

    type=1;
    while (fgets (input, 8000, fptr)!=NULL){
        if(isEmptyLine(input)){
            continue;
        }
        int len = strlen(input);
        int qw=0;
        int source, destination;
        int flag=0;
        for(int i=0;i<len;i++){
            if(input[i]==',' && isUselessChar(input[i])){
                continue;
            }
            
            int num=0;
            if(!flag){
                flag=1;
                while(i<len && input[i]!=':'){
                    if(isUselessChar(input[i])){
                        i++;
                        continue;
                    }
                    num = num*10 + ((int)(input[i]-'0'));
                    i++;
                }
                i++;
                source = num;
            }
            num=0;
            bool isthere=0;
            while(i<len && input[i]!=','){
                if(isUselessChar(input[i])){
                    i++;
                    continue;
                }
                isthere=1;
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            destination = num;
            
            if(isthere){
                addEdge(graph, source, destination, type);
                addEdge(*reversedGraph, destination, source, 0);
            }
        }     
    }
    return graph;
}

/*  
    heapify() - funtion to heapify maxHeap
    arr   -   arr of pair(MaxHeap)
    n   -   max Length of MaxHeap
    i   -   heapify position

*/
void heapify(struct pair arr[], int n, int i) 
{ 
    int largest = i; 
    int l = 2*i + 1;  
    int r = 2*i + 2;  
  
    if (l < n && arr[l].val > arr[largest].val) 
        largest = l; 
  
    if (r < n && arr[r].val > arr[largest].val) 
        largest = r; 
  
    if (largest != i) 
    {
        struct pair tmp;
        tmp =arr[largest];
        arr[largest] = arr[i];
        arr[i] = tmp;
        
        heapify(arr, n, largest); 
    } 
} 
  
/*  
    sort() - HeapSort
    arr   -   arr of pair(MaxHeap)
    n   -   max Length of MaxHeap

*/
void sort(struct pair arr[], int n) 
{ 
    for (int i = n / 2 - 1; i >= 0; i--) 
        heapify(arr, n, i); 
  
    for (int i=n-1; i>0; i--) 
    {   
        struct pair tmp;
        tmp = arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;
        
        heapify(arr, i, 0); 
    } 
} 



int main(int argc, char** argv){
    
	if (argc != 5){ 
		printf("Please pass Exactly 4 CLI argument as testcase file, userIndx, K and L \n"); 
		exit(0); 
	}
    
    FILE *fptr; //file object 
	
    //open file in read mode
	fptr = fopen(argv[1], "r"); 
	int U = atoi(argv[2]);
    int K = atoi(argv[3]);
    int L = atoi(argv[4]);
    //check if file found or not
    if (fptr == NULL){ 
		printf("Cannot open file \n"); 
		exit(0); 
	} 
	
	int userCount=0,contentCount=0;
    struct Graph* graph = NULL;
    struct Graph* reversedGraph = NULL;
    
    //parse input file and store into graph
    graph = parseInput(fptr, graph, &reversedGraph, &userCount, &contentCount);    

	fclose(fptr); 
 
    //for U
    struct pair distance[contentCount];
    
    //find all content at k distnace from user U
    int vaildContentCounter=0;
    distanceLContentNodes(graph, U, distance, K, userCount, contentCount, &vaildContentCounter);
    

    bool isAlreadyFriend[userCount]; //friends with U
    bool userInRace[userCount];     //could be recommonded
    int userScore[userCount];       // Score of users
    bool finalUserList[userCount];  // Final list of Valid User that can be recommonded
    
    //intiallization
    for(int i = 0; i < userCount ; i++){
        userScore[i]=0;
        isAlreadyFriend[i]=0;
        userInRace[i]=0;
        finalUserList[i]=0;
    }

    //already Firends
    struct Node* tmp = NULL;
    tmp = graph->array[U].head;
    while(tmp!=NULL){
        if(tmp->type==0)
            isAlreadyFriend[tmp->dest] = 1;
        tmp = tmp->next;
    }
    
    // from each content(let ci), find user(let uj) at dist 1 and then 
    // find all users(let up, up+1... uq) at distance k-1 from those users
    // because for all those users(up, up+1... uq) that content(ci) will be at max 'k' distance away
    int county=0;
    for(int i = 0; i < vaildContentCounter ; i++){
        int ci = distance[i].idx;
        tmp = reversedGraph->array[ci].head;
		while(tmp!=NULL){
            int u = tmp->dest;
            //user at k-1 distnace from u
            connectedUserNodes(graph, u, userInRace, K-1, userCount);
			tmp = tmp->next;
		}

        // to make sure no user is counted double times from the same user
        // Increase 'userScore' for all users which 
        // have this content(ci) at atmost 'k' distance away from them
        for(int i = 0; i < userCount ; i++){
            if(userInRace[i] && !(isAlreadyFriend[i] || i==U)){
                userScore[i]++;
                if(!finalUserList[i]){
                    county++;
                    finalUserList[i] = 1;
                }    
            }
            userInRace[i]=0;
        }
    }

    //to make array of pair so that we can sort it without losing contentIdx value
    struct pair userScoreWithId[county];
    int j=0;
    for(int i = 0; i < userCount ; i++){
        if(finalUserList[i]){
            userScoreWithId[j].idx = i;
            userScoreWithId[j++].val = userScore[i];
        }
    }

    //sort using own HeapSort
    sort(userScoreWithId, county);
    
    //print in Decreasing Order
    for(int i = county-1; i >=0 && L>0 ; i--){
        printf("User: %d, Score: %d\n",userScoreWithId[i].idx,userScoreWithId[i].val);
        L--;   
    }

    //free memory
    deallocateGraph(graph);
    deallocateGraph(reversedGraph);
    return 0;
} 