/*
Name:            Vipray Jain
Assignment No. : 2
Task :           1
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
  
// Graph is made up of List of 'V' heads
// ith heads points the the linklist of all the nodes connected with node 'i'
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
		g->array[i].head = NULL;	//Intiallize head of each List by NULL
	}
	
	return g;
}


/*  
    intiallizeQueue(v) - funtion to intiallize Queue
*/
struct Queue* intiallizeQueue(int V){
	struct Queue* q =  (struct Queue*)(malloc(sizeof(struct Queue)));
	
    //array of linked list to store adjacency list
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

    // For Friendship Links(undirected), the input file itself contains double entry for each edges
    // so no need to add e->s here 

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
    topKMinDist() - funtion to run BFS and find top k min distance content from given src
    
    g   -   instant of Graph
    src -   user from which k min distance content is needed
    dist    -   struct array to store content node and its distance
    k   -   given parameter(limit of max number of content to recommend )
    userCount    -   used to ensure no closed user node is further visited
    contentCount    -   used to ensure no closed content node is further visited
    distLen  -   length of dist array

*/

void topKMinDist(struct Graph* g, int src, struct pair dist[], int k, int userCount, int contentCount, int *distLen)
{
    struct Queue *q = intiallizeQueue(userCount);

    bool visitedUser[userCount];
    bool visitedContent[contentCount];

    // initallization
    for (int i = 0; i < userCount; i++){
        visitedUser[i] = false;
    }

    // initallization
    for (int i = 0; i < contentCount; i++){
        visitedContent[i] = false;
    }

    //mark src user visited
    visitedUser[src]=1;
    enqueue(q,src,0);
    
    int nextDepth=0;
    
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

            // If its a content Node  
            if(t==1 && !visitedContent[v]){

                visitedContent[v]=1;
                // Own shared contents will always be at distance of one from itself
                if(nextDepth!=1){   //  not own shared content
                    dist[(*distLen)].idx = v;    //content number
                    dist[(*distLen)++].val = nextDepth; //distance of content
                }
                
                //break if got top valid k content
                if((*distLen)==k){
                    flag=1;
                    break;
                }
                // Content nodes do not have any outgoing edge
                // So there is no use of pushing them into the queue

            }
            
            // If its a User Node
            else if(t==0 && !visitedUser[v]){
                //put user into Queue
                visitedUser[v]=1;
                enqueue(q,v,nextDepth);
            }

            tmp = tmp->next;
        }
        //stop traversing once valid k contents are found
        if(flag){
            break;
        }
    }
    //free memory allocated to q
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
struct Graph* parseInput(FILE *fptr, struct Graph* g, int *uc, int *cc){
    char input[8005];   // one line of file at a time
    
    while(fgets (input, 8000, fptr)!=NULL){
        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }
        
        int len = strlen(input);
        for(int i=0;i<len;i++){
            if(isUselessChar(input[i])){
                continue;
            }
            int num=0;
            while(i<len){   //string to int
                if(isUselessChar(input[i])){
                    i++;
                    continue;
                }
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            *uc = num;// number of user
        }
    
        break;  
    }

    int V = *uc; 
    struct Graph* graph = intiallizeGraph((V));
	

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
                source = num;  //source
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
            destination = num;  //destination
            if(isthere){
                addEdge(graph, source, destination, type);
            }
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
            }
        }     
    }
    return graph;
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
    
    //parse input file and store into graph
    graph = parseInput(fptr, graph, &userCount, &contentCount);    

	fclose(fptr); 
 
    //to store distnace of top K contents for user U
    struct pair distance[K];

    int distanceLen = 0;

    //funtion to get top k min distance content from U
    topKMinDist(graph, U, distance, K, userCount, contentCount, &distanceLen);

    //printf("%d Recommonded Contents for user %d is:\n",K,U);

    for(int v = 0; K>0 && v<distanceLen; v++){
        if(1 || distance[v].val!=-1){
            printf("Content: %d, Distance: %d\n",distance[v].idx,distance[v].val);
            K--;
        }
    }
    deallocateGraph(graph);
} 