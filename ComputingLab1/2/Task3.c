/*
Name:            Vipray Jain
Assignment No. : 2
Task :           3
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
    int privacy; //link privacy
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
void addEdge(struct Graph* g, int s, int e, int type, int privacy){
	//s->e
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->dest = e;
	newNode->type = type;
    newNode->privacy = privacy;
    
	//add node in AdjList of node 's' (in front)
	newNode->next = g->array[s].head;
	g->array[s].head = newNode;
    
	/*
    if(type==0){//only user in undirected
        //e->s
        newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = s;
        newNode->type = type;
        //add node in AdjList of node 'e' (in front)
        newNode->next = g->array[e].head;
        g->array[e].head = newNode;
    }	
    */

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
    
    //printf("%d--->\n",q->front);
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
    //printf("%d+++>\n",q->front);
    return qn;
}

/*  
    topKMinDist() - funtion to run BFS and find top k min distance content from given src
    
    q   -   instant of Queue
    src -   user from which k min distance content is needed
    dist    -   struct array to store content node and its distance
    publicContents  -   content that is already public
    k   -   given parameter(limit of max number of content to reccomond )
    userCount    -   used to ensure no closed user node is further visited
    contentCount    -   used to ensure no closed content node is further visited
    distLen  -   length of dist array

*/
void topKMinDist(struct Graph* g, int src, struct pair dist[], bool publicContents[], int k, int userCount, int contentCount, int *distLen)
{
    //make queue
    struct Queue *q = intiallizeQueue(userCount);

    bool visitedUser[userCount];
    bool visitedContent[contentCount];

    //initallization
    for (int i = 0; i < userCount; i++){
        visitedUser[i] = false;
    }

    for (int i = 0; i < contentCount; i++){
        visitedContent[i] = false;
    }

    visitedUser[src]=1;
    enqueue(q,src,0);
    
    int nextDepth=0;
    int breakCounter=0;
    
    // Find shortest path for all vertices
    int depthLimit = INT_MAX;
    while(!isEmpty(q))
    {
        int type;
        struct QueueNode qn = dequeue(q);
        int u = qn.val;
        nextDepth = qn.depth + 1;
        visitedUser[u] = true;
        
        struct Node* tmp = g->array[u].head;
        int v,t,p,decrease;
        bool flag=0,flag2;

        //check all the neighbour of u
        while(tmp!=NULL){
            v = tmp->dest;
            t = tmp->type; 
            p = tmp->privacy; 
            
            if(t==1 && !visitedContent[v]){
                visitedContent[v]=1;

                if(nextDepth!=1 && !publicContents[v]){// run if not own shared content
                    if(depthLimit < nextDepth){
                        flag=1;
                        break;
                    }
                    //if depth is two and p is also 2 then dist should be 1 not zero
                    if(nextDepth - p == 0)
                        p=p-1;

                    dist[(*distLen)].idx = v;    //content number
                    dist[(*distLen)++].val = nextDepth - p; //distance of content from src
                }    
                
                //keep track of max depth
                //so we check all the content at that node
                // It is important to check for all the nodes with depth = nextDepth
                // even after getting top k
                // Because, suppose last pop node have nextDepth 3 and p = 1
                // it will get added with distance of 2
                // Now suppose next node in queue have nextDepth 3 and p = 2
                // then its distance will be 1
                // and if we wont add this to 'dist' array then our soln will be wrong
                if((*distLen) == k){
                    depthLimit = nextDepth;
                }
            }

            else if(t==0  && !visitedUser[v]){
                visitedUser[v]=1;
                enqueue(q,v,nextDepth);
            }
            
            tmp = tmp->next;
        }
        if(flag){
            break;
        }
    }
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
    uc  -   pointer to maintain of userCount
    cc  -   pointer to maintain of contentCount

*/
struct Graph* parseInput(FILE *fptr, struct Graph* graph, bool **publicContents, int *uc, int *cc){
    char input[10005];   // one line of file at a time
    
    while(fgets (input, 10000, fptr)!=NULL){
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
    //printf("UserCount:%d\n",*uc);

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
                addEdge(graph, source, destination, type, -1);
            
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
    
    *publicContents = (bool*)malloc((*cc)*(sizeof(bool)));


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
            i++;
            
            int privacy;
            num=0;
            while(i<len && input[i]!=';' ){
                if(isUselessChar(input[i])){
                    i++;
                    continue;
                }
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            privacy = num;
            if(privacy==0 && isthere){//public
                (*publicContents)[destination]=1;
            }
            if(isthere)
                addEdge(graph, source, destination, type, privacy);
        }     
    }
    return graph;
}

/*  
    heapify() - funtion to heapify minHeap
    arr   -   arr of pair(MinHeap)
    n   -   max Length of MinHeap
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
    arr   -   arr of pair(MinHeap)
    n   -   max Length of MinHeap

*/
void sort(struct pair arr[], int n) 
{ 
    for (int i = n / 2 - 1; i >= 0; i--) 
        heapify(arr, n, i); 
  
    for (int i=n-1; i>0; i--) 
    {   
        struct pair tmp;
        tmp =arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;
        
        heapify(arr, i, 0); 
    } 
} 


int main(int argc, char** argv){
    
    if (argc != 4){ 
		printf("Please pass Exactly 3 CLI argument as testcase file, userIndx and K \n"); 
		exit(0); 
	}
    
    FILE *fptr; //file object 
	
    //open file in read mode
	fptr = fopen(argv[1], "r"); 
	int U = atoi(argv[2]);
    int K = atoi(argv[3]);
    //check if file found or not
    if (fptr == NULL){ 
		printf("Cannot open file \n"); 
		exit(0); 
	} 
	
	int userCount=0,contentCount=0;
    struct Graph* graph = NULL;
    bool* publicContents = NULL;
    
    //parse input file and store into graph
    graph = parseInput(fptr, graph, &publicContents, &userCount, &contentCount);    

	fclose(fptr); 
    
    //already shared contents by user U
    int isAlreadyShared[contentCount];
    for(int i = 0; i < contentCount ; i++){
        isAlreadyShared[i]=0;
    }

    struct Node* tmp = NULL;
    tmp = graph->array[U].head;
    while(tmp!=NULL){
        if(tmp->type==1)
            isAlreadyShared[tmp->dest] = 1;
        tmp = tmp->next;
    }

    int i=0;
    int publicContentsCount=0;

    //print all at distance 1
    while (K>0 && i<contentCount){
        if(publicContents[i] && !isAlreadyShared[i]){
            printf("Content:%d, Distance:%d\n",i,1);
            K--;
        }
        i++;
    }

    // no more recommondation required, stop here
    if(K==0){
        return 0;
    }
    
    //distance of content that can be recommonded
    struct pair distance[contentCount]; 
    int toRcmdCounter=0;
    topKMinDist(graph, U, distance, publicContents, K, userCount, contentCount, &toRcmdCounter);

    //sort using HeapSort
    sort(distance, toRcmdCounter);
    
    for (i = 0; K>0 && i<toRcmdCounter; i++){
        if(distance[i].val != -1){
            printf("Content: %d, Distance: %d\n",distance[i].idx,distance[i].val);
            K--;
        }
        
    }

    //free memory
    deallocateGraph(graph);
    free(publicContents);

    return 0;
} 