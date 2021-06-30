/*
Name:            Vipray Jain
Roll No. :       20CS60R37
Assignment No. : 5
Task :           1,2,3
*/



#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h> // library for fcntl function
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define BUFSIZE 80	

/*
    Node    -   childNode
*/
struct Node{
    int data;
    struct Node* next; 
};

/*
    ProLog  -   Structure to hold data related to each process
*/
struct ProLog{
    int myId;
    int childCount;
    //int parPipe[2];
    int **childPipe;
 
};

/*
    AdjList  -   Structure to hold data intially in adjacency list format
*/
struct AdjList{
    int myId;
    int parId;
    int childCount;
    struct Node *children;
};




/*
    indexMapper()  -   function to find the index for perticular value
    p              -   Adjacency list in which index to we looked
    n              -   no of entries in 'p'
    d              -   values whose index is to be found
*/
int indexMapper(struct AdjList p[], int n, int d){
    for(int i=0;i<n;i++){
        if(p[i].myId == d || p[i].myId == -1){//-1 represent empty location
            return i;
        }
    }
}


/*  
    intiallizeProLog() - funtion to intiallize Process log for each process
    a              -   Adjacency list in which index to we looked
    idx            -   index of process tag in 'a'
*/
struct ProLog* intiallizeProLog(struct AdjList a[], int idx){
	struct ProLog* p = (struct ProLog*)(malloc(sizeof(struct ProLog)));
    
    p->childCount = a[idx].childCount;
    p->myId = a[idx].myId;
    //p->parPipe[0] = -1;
    //p->parPipe[1] = -1;

    int n = a[idx].childCount;
    
    //n+2 because may need to increase child size in case of pipe changing
    p->childPipe =  (int**)malloc((n+2)*sizeof(int*));
    for(int j=0;j<n+2;j++){
        p->childPipe[j] =  (int*)malloc(2*sizeof(int));
    }

    //parent pipe pushed in the last
    p->childPipe[n][0] = -1;
    p->childPipe[n][1] = -1;
    
    return p;
}



/*  
    intiallizeAdjList(v) - funtion to Adjacency list
    l              -   Adjacency list in which index to we looked
    n              -   no of entries in 'p'
*/
void intiallizeAdjList(struct AdjList l[],int n){
	for(int i=0;i<n;i++){
        l[i].childCount=0;
        l[i].myId = -1;
        l[i].parId = -1;
        l[i].children = NULL;
    }
}


/*  
    makeStruct() - funtion to add data in Adjacency list
    p              -   Adjacency list in which index to we looked
    n              -   no of entries in 'p'
    c              -   child of 'par'
    par            -   parent of 'c'

*/
void makeStruct(struct AdjList p[], int n, int c, int par){
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->data = c;
	
    // find out the Id for the parent
    int idx = indexMapper(p, n, par);
    
    newNode->next = p[idx].children;
    p[idx].children = newNode;
    p[idx].childCount++;
    p[idx].myId = par;

    // find out Id for child
    idx = indexMapper(p, n, c);
    p[idx].myId = c;
    p[idx].parId = par;
	
}


/*  
    viewGraph()    -   funtion to view Adjacency list
    p              -   Adjacency list in which index to we looked
    n              -   no of entries in 'p'

*/
void viewGraph(struct AdjList p[], int n){
	int i=0;
	struct Node* tmp = NULL;
	printf("AdjLists:\n\n");
	for(i=0; i<n; i++){
		printf("Tag%d, Par%d, ChildC%d: ",p[i].myId,p[i].parId,p[i].childCount);
        tmp = p[i].children;
		while(tmp!=NULL){
			printf("%d, ",tmp->data);
			tmp = tmp->next;
		}
		printf("\n");
	}
}

/*  
    buildProcessTree()    -   funtion to build process tree using fork()
    p              -   Adjacency list in which index to we looked
    n              -   no of entries in 'p'
    r              -   Pointer to current process log
    rDash          -   Temp var for Process Log

*/
void buildProcessTree(struct AdjList p[], struct ProLog *r, struct ProLog rDash, int n){
    if(r->childCount == 0){
        return;
    }


    int root = r->myId;
    int tag = root;
    int pid;
    int i=0;
    int idx = indexMapper(p, n, root);      //find index of root in AdjList
	
    // start iterating for childs of root(recursively)
    struct Node* tmp = NULL;
    tmp = p[idx].children;

    while(tmp!=NULL){
        
        int childIdx = indexMapper(p, n, tmp->data);
        
        //Open two pipes
        /*  
            Each pipe has 2 ends. what we send at 0th index will get recieved at 1th index
            On one pipe you can either read or right
            so you can speak from one of its end and listen from other end
            As in our problem, we need to communicate from both ends
            So we will make two pipes
            Here,
            pip[0] will used by Parent to send msg to child
            So parent will close pip[1], Child will listen on pip[1] so it will close pip[0]
            
            pip2[0] will used by Child to send msg to child
            So Child will close pip2[1], Parent will listen on pip2[1] so it will close pip2[0]
            

        */
        int pip[2];
        int pip2[2];
        pipe(pip);
        pipe(pip2);

        //assign child pipes
        r->childPipe[i][0] = pip[0]; // pipe to send msg to child
        r->childPipe[i++][1] = pip2[1]; // pipe to recv msg from child
        
        //r' make a copy of child process details from adjList
        rDash.childCount = p[childIdx].childCount;
        rDash.myId = p[childIdx].myId;
        
        //Code section For child, Here we are creating new process for child
        // And child and parent are connected using pipes
        if((pid = fork())==0){
            // Here we are doing i set back to zero(FOR child)
            // So that when the above While gets executed as child then at that time
            // i should be 0 so that we can visit all its child one by one.
            // THE BEAUTY OF FORKING
            i=0;

            //similar to i, here we areassigning child's attributes to the pointer 'r'

            //free memory in childs local version so that we can assign new memory
            for(int j=0;j<r->childCount+1;j++){
                free(r->childPipe[j]);
            }
            free(r->childPipe);


            r->childCount = rDash.childCount;
            r->myId = rDash.myId;
            
            
            int nn = rDash.childCount;
            
            r->childPipe =  (int**)malloc((nn+2)*sizeof(int*));
            for(int j=0;j<nn+2;j++){
                r->childPipe[j] =  (int*)malloc(2*sizeof(int));
            }

            //assign parent pipes
            r->childPipe[nn][0] = pip2[0];
            r->childPipe[nn][1] = pip[1];

            //close other ends
            close(pip[0]);
            close(pip2[1]);

            //recur to childs
            buildProcessTree(p, r, rDash, n);   
            
            return;
        }
        else{
           //close pipes for parent
           close(pip[1]);
           close(pip2[0]);
        }

        tmp = tmp->next;
    }
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
    parseInput()   -   funtion to parse input file
    s              -   input file name
    *nodes         -   to return number of nodes in process tree

*/
struct AdjList* parseInput(char *s, int *nodes){
    FILE *fptr; //file object 
	
    //open file in read mode
	fptr = fopen(s, "r");
    if (fptr == NULL){ 
		printf("Cannot open file \n"); 
		exit(0); 
	} 

    char *input = (char*)malloc(50*sizeof(char));   
    int noOfEdges=0;//Count edges
    while(fgets (input, 50, fptr)!=NULL){
        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }

        noOfEdges++;    //count lines(edges) in file
    }
    fclose(fptr);

    *nodes = noOfEdges + 1;
    struct AdjList* pL = (struct AdjList*)(malloc((*nodes)*sizeof(struct AdjList)));
    intiallizeAdjList(pL,(*nodes));

    //open file in read mode
	fptr = fopen(s, "r"); 
    if (fptr == NULL){ 
		printf("Cannot open file \n"); 
		exit(0); 
	}

    char* tok;
    int c,par;

    while(fgets (input, 50, fptr)!=NULL){
        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }
        
        tok = strtok(input, ",");
        c = atoi(tok);
        
        tok = strtok(0, ",");
        par = atoi(tok);

        //insert into structure
        makeStruct(pL, (*nodes), c, par);
        
    }
    fclose(fptr);
    return pL;
}


/*  
    SUM()   -   funtion to add two numbers
    a              -   num1
    b              -   num2

*/
int SUM(int a, int b){
    return a+b;
}


/*  
    election()   -   funtion to find leader
    pL           -   process log of current process
    root         -   root to process tree
    *loc         -   loc(side) from where we found minimum

*/
int election(struct ProLog* pL, int root, int *loc){
    char line[BUFSIZE];
    char line2[BUFSIZE];
    
    bool wsp = false;
    int neighbours = pL->childCount+1;//+1 as parent is also a neighbour

    //Code for initiator(root)
    if(pL->myId == root){
        neighbours--;   // Root dont have parent
        wsp = true;
        //sending wakeupCall to neighbours
        for(int r=0;r<neighbours;r++){
            sprintf(line,"WakeUp\n");
            write(pL->childPipe[r][1], line, BUFSIZE);
            line[0]='\0';
        }
    }

    for(int r=neighbours-1; r>=0; r--){
        //reading wakeUpCall from parent
        read(pL->childPipe[r][0], line2, BUFSIZE);
        line2[0]='\0';
        if(!wsp){   
            wsp = true;
            //sending wakeupCall to neighbours
            for(int r=0;r<neighbours;r++){
                sprintf(line,"WakeUp\n");
                write(pL->childPipe[r][1], line, BUFSIZE);
                line[0]='\0';
            }
        }
    }

    int neigh = neighbours;
    neigh--;
    int vp = pL->myId;
    int value;
    for(int r=0; r<neigh; r++){
        /*
            reading Token from neighbours(except one ie parent).
            In case of root, that one become any one of its child.
            So Leaf nodes wont have any neighbout other than parent So they wont get in this block
        */
        read(pL->childPipe[r][0], line2, BUFSIZE);
        sscanf(line2,"%d",&value);
        line2[0]='\0';
        
        //keeping track of direct from where we got the minimun
        if(value<vp){
            vp = value;
            *loc = r;
        }
    }
    
    //sending token to the remaing one
    sprintf(line,"%d\n",vp);
    write(pL->childPipe[neigh][1], line, BUFSIZE);
    line[0]='\0';
    
    //reading token from the remaing one
    read(pL->childPipe[neigh][0], line2, BUFSIZE);
    sscanf(line2,"%d",&value);
    if(value<vp){
        vp = value;
        *loc = neigh;
    }
    
    int state = 0; //0-sleep    1-leader    2-lost
    if(vp == pL->myId){
       state=1;
    }
    else{
        state=2;
    }


    printf("I am %d, My leader is %d\n",pL->myId, vp);
    
    // send token to everyone other than the one from which u last get it.
    for(int r=0; r<neigh; r++){    
        sprintf(line,"%d\n",vp);
        write(pL->childPipe[r][1], line, BUFSIZE);
        line[0]='\0';
    }

    //return leader
    return vp;
}



/*  
    sumCount()   -   funtion to find sum of all process's ID
    pL           -   process log of current process
    leader       -   leader to process tree
*/
int sumCount(struct ProLog* pL, int leader){
    
    char line[BUFSIZE];
    char line2[BUFSIZE];

    int neighbours = pL->childCount+1;
    if(pL->myId == leader){
        neighbours--;
    }

    int neigh = neighbours;
    if(pL->myId != leader){
        neigh--;
    }

    int vp = pL->myId;
    int value;

    //same as did for token sending in election function
    for(int r=0; r<neigh; r++){
        read(pL->childPipe[r][0], line2, BUFSIZE);
        value=0;
        sscanf(line2,"%d",&value);
        line2[0]='\0';
        vp = SUM(vp,value);
    }

    
    if(pL->myId != leader){
        sprintf(line,"%d\n",vp);
        write(pL->childPipe[neigh][1], line, BUFSIZE);
        line[0]='\0';
    }

    if(pL->myId == leader){
        read(pL->childPipe[neigh][0], line2, BUFSIZE);
        value=0;
        sscanf(line2,"%d",&value);
        vp = SUM(vp,value);
    }

    //return sum
    return vp;

}


/*  
    letItTerminate()   -   funtion to ask every process to terminate
    pL           -   process log of current process
    leader       -   leader to process tree
*/
void letItTerminate(struct ProLog* pL, int leader){
    
    char line[BUFSIZE];
    char line2[BUFSIZE];
    int neighbours = pL->childCount;
    
    //it is similar to the wakeUp call part of the election function

    //other that leader all other will first read the message and then write to their modified children
    if(pL->myId != leader){
        read(pL->childPipe[pL->childCount][0], line2, BUFSIZE);
        line2[0]='\0';
    }

    //leader only write to its modified children
    for(int r=0;r<neighbours;r++){
        sprintf(line,"%d,%d\n",-100,pL->myId);
        write(pL->childPipe[r][1], line, BUFSIZE);
        line[0]='\0';
    }

    //then every process will wait for its orginal childs to be terminate  
    // We have changed the pipe only
    // But during the buildProcessTree call, the hierarchy of child and parent remains same
    // So when wait is called, it will wait for all its child forked processes to terminate
    // Thus we got termination in orginal order.  
    int kl;
    while ((kl = wait(NULL)) > 0);
    printf("I am agent %d and i am going to terminate\n",pL->myId);
    
    //finally processes will terminate one by one
    exit(0);

}

/*  
    findRoot()   -   funtion to find root of by AdjList
    p            -   AdjList
    n            -   size of AdjList
*/
int findRoot(struct AdjList* p, int n){
    for(int i=0;i<n;i++){
        if(p[i].parId == -1){//-1 represent no par
            return p[i].myId;
        }
    }
}


int main(int argc, char** argv){
    
    if (argc != 2){ 
		printf("Please pass Exactly 1 CLI argument as testcase file \n"); 
		exit(0); 
	}
    
    int n,root;
    struct AdjList* al = parseInput(argv[1], &n);   //file to adjList
    
    root  = findRoot(al,n);     //root
    int idx = indexMapper(al,n,root);   //index of root

    // root will we run in "main" process
    struct ProLog* pL = intiallizeProLog(al,idx);   //process log of current process
    struct ProLog pLDash;  
    
    int mainID = getpid();
    
    buildProcessTree(al, pL, pLDash, n);    //build tree
    // Code From hereafter will we executed by all the forked process
    
    
    int loc;
    int leader = election(pL, root, &loc);  //leader
    //printf("\n");

    /*
        Chainging direction of pipe according to the leaders(min tag) postion
        loc will help in doing so
    */
    
    //for leader make him feel like root by making every pipe as its child
    if(pL->myId == leader){
        pL->childCount++;
        pL->childPipe[pL->childCount][0] = -1;
        pL->childPipe[pL->childCount][1] = -1;
    }
    else{
        // for root change one (in direction of leader) of its child's pipe as its parent's pipe
        if(pL->childPipe[pL->childCount][0] == -1){//root
            int apip = pL->childPipe[loc][0];
            int bpip = pL->childPipe[loc][1];

            pL->childPipe[loc][0] = pL->childPipe[pL->childCount-1][0];
            pL->childPipe[loc][1] = pL->childPipe[pL->childCount-1][1];

            pL->childPipe[pL->childCount-1][0] = apip;
            pL->childPipe[pL->childCount-1][1] = bpip;
            pL->childCount--;
        }

        //swap pipes to points towards the process which gave the leader(min tag)
        else{
            int apip = pL->childPipe[loc][0];
            int bpip = pL->childPipe[loc][1];
            
            pL->childPipe[loc][0] = pL->childPipe[pL->childCount][0];
            pL->childPipe[loc][1] = pL->childPipe[pL->childCount][1];

            pL->childPipe[pL->childCount][0] = apip;
            pL->childPipe[pL->childCount][1] = bpip;
            
        }
    }

    char line[BUFSIZE];
    char line2[BUFSIZE];
    
    sleep(2);
    
    int sum = sumCount(pL,leader);
    if(pL->myId ==leader){  //leader will print the sum
        printf("Sum: %d\n",sum);
    }
    char rtds[BUFSIZE];

    //printf("\n");
    sleep(2);
    
    letItTerminate(pL,leader);  //termination of all process, initated by leader
    
    return 0;
}
