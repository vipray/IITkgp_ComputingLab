/*
Name:            Vipray Jain
Assignment No. : 1
Task :           3
*/



#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#define p 1003  //first prime number bigger than max vertices(could be)


// Need to maintain a Chain at each location of hash table as to handle collisions

//stores the info about substring
struct Node{ 
    int sId;    //start index
    int eId;    //end index
    int matchCount; //how many have collided with this node
    
    // this field make sure that matchCounter wont get increased more than once for same path 
    // Example 1212 here, for length 2 , 12, 12 comes twice
    // in this case matchCount get Increased twice and that will give wrong outputs
    int pathnoOfLastMatchedPath; // path no of last matched path  
    struct Node* next; 
}; 

//chains(block of HashTable) of Nodes used in HashTable in case of match
struct Chain { 
    struct Node* head;  
}; 
  
//HashTable to store substring(Node)
struct HashTable { 
    int size ; 
    struct Chain* chains; 
};

/*  
    intiallizeHashTable() - funtion to intiallize HashTable
*/
struct HashTable* intiallizeHashTable(){
	struct HashTable* ht =  (struct HashTable*)(malloc(sizeof(struct HashTable)));
	ht->size = p;
	
	ht->chains = (struct Chain*)(malloc(p * sizeof(struct Chain)));	//allot p chains(blocks)
	
	int i=0;
	for(i=0;i<p;i++){
		ht->chains[i].head = NULL;	//Intiallize head of each chain by NULL
	}
	
	return ht;
}

/*  
    isMatchingWithOtherPath() - funtion to check match

    ht - Instance of HashTable
    hashVal - Value at which match need to be checked
    paths[][1001] - Details of all paths
    pathno  -   Path No. with which match need to be checked
    strIdx  -   starting index of substring
    endIdx  -   ending index of substring
*/
int isMatchingWithOtherPath(struct HashTable* ht, int hashVal, int paths[][1001], int pathno, int strIdx, int endIdx){
    struct Node* tmp = ht->chains[hashVal].head;
    int f=0;
    int len = endIdx - strIdx + 1;
    while(tmp){
        f=0;
        int sId = tmp->sId;
        int eId = tmp->eId;
        for(int i = 0; i<len; i++){
            if(paths[0][sId+i]!=paths[pathno][strIdx+i]){ //means not matching
                f=1;
                break;
            }
        }
        if(!f){
            if(pathno==0){
                return 1;
            }
            if(tmp->pathnoOfLastMatchedPath!=pathno){
                tmp->matchCount += 1; //matchCount has increased
                tmp->pathnoOfLastMatchedPath = pathno;
            }
            return tmp->matchCount;   //match, send matchCount
        }

        tmp = tmp->next;
    }
    return 0;   // no match
}

/*  
    insert() - insert subpaths endpoints in hashtable

    ht - Instance of HashTable
    hashVal - Value at which match need to be checked
    paths[][1001] - Details of all paths
    strIdx  -   starting index of substring
    endIdx  -   ending index of substring
*/
void insert(struct HashTable* ht, int hashVal, int paths[][1001], int strIdx, int endIdx){
    
    // to check if already a substring hashed by path 0,contains it again
    // Eg-> 1212 here, for length 2 , 12, 12 comes twice
    int matchCount = isMatchingWithOtherPath(ht, hashVal, paths, 0, strIdx, endIdx);
    if(matchCount){
        return ;
    }
    
    //assigning values to new node
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->matchCount = 0;
    newNode->eId = endIdx;
    newNode->sId = strIdx;
    newNode->pathnoOfLastMatchedPath = 0;
    
	//add node in chain with hash 'hashVal' (in front)
	newNode->next = ht->chains[hashVal].head;
	ht->chains[hashVal].head = newNode;
	
}

/*  
    calculateRollingHashAtOnce() - funtion to calculate Hash using rolling Hash
    
    ht - Instance of HashTable
    paths[][1001] - Details of all paths
    pathLen -   Length of current path
    pathno  -   Path No. with which match need to be checked
    subLen  -   length for which substrings hash need to be calculated
    userCount - No of total user(that will help in knowing whther all users path get collided or not)
    tmpNode  -   To take back the endpoints of Matching substring to the main() function
    d   -   variable for rolling hash(largest vertices value)

*/
int calculateRollingHashAtOnce(struct HashTable* ht, int paths[][1001], int pathLen, int pathno, int subLen, int userCount, struct Node* tmpNode, int d){
    int dPowI = 1;// d^0=1
    int hashVal = 0;

    int f=0;
    // hash for very first substring
    for (int i=0; i<subLen; i++) { 
        hashVal = (((hashVal* d)%p)+ ((paths[pathno][i]*d) % p))% p;
        dPowI = (dPowI*d)%p;
    }

    // -1 is to cover the first substring also which is generated above
    for (int i = -1; i<pathLen - subLen; i++) { 
        if(i != -1){
            //hash using rollingHash algo
            // here we are removing the ith letter and adding the (subLen+i)th letter
            hashVal = ( ((d*(hashVal - paths[pathno][i]*dPowI + p*dPowI))%p) + (d*paths[pathno][subLen+i]) )% p;
            //if goes negative
            if(hashVal<0){
                hashVal+=p;
            }
        }

        if(pathno == 0){
            /* If this is the first path(pathno = 0)
                means Hash Table is empty
                So add entry into it           
            */
            insert(ht, hashVal, paths, i+1, subLen+i);
        }
        else{
            // if 0, then no match
            // otherwise return the match count of node with which match occured
            int matchCount = isMatchingWithOtherPath(ht, hashVal, paths, pathno,  i+1, subLen+i);
            if(matchCount == pathno){ // means till now, all the paths have this subPath
                f=1;
                if(matchCount == userCount-1){
                    tmpNode->sId = i+1;
                    tmpNode->eId = subLen+i;
                    return 2;
                }
            }
        }

    }
    /*
        f=0 - no match for this user with any substring of this length
              So it is of no use to check for other user with same length

        f=1 - there is atleast on substring of this length for which
              till now all the users have collided. So keep checking for other users

        f=2 - all user collided with some substring of this length 
              So Solution found, terminate the program
    */
    return f;

}


/*
    preprocessingInputFile() -  Preprocess Input File to extract paths into arrays
    fptr                     -  File Pointer
    paths                    -  Array to store paths
    pathLen                  -  Array to store path lengths
    largestVerticesId        -  Id of largest Vertex among all paths
    smallestPathLen          -  length of the smallest path among all paths

*/
void preprocessingInputFile(FILE* fptr, int paths[][1001], int pathLen[], int* largestVerticesId, int* smallestPathLen){
    char input[8005];   // line of file at a time
    int j=0,len,k=0;

    // File preprocessing
    while (fscanf(fptr, "%s", input) != EOF){
        int len = strlen(input);
        k=0;
        int qw=0;
        //to remove values before ":"
        while(input[qw]!=':'){
            qw++;
        }
        for(int i=qw+1;i<len;i++){
            if(input[i]==',')
                continue;

            //String to number
            int num=0;
            while(i<len && input[i]!=','){
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            paths[j][k++] = num;
            if((*largestVerticesId)<num){
                *largestVerticesId = num;
            }
        }
        pathLen[j]=k;
        if((*smallestPathLen)>k){
            *smallestPathLen = k;
        }   
        j++;      
            
    }
}



int main(int argc, char** argv){

    //initiallize hashTable
	struct HashTable* ht = intiallizeHashTable();   


    if (argc != 2){ 
		printf("Please pass Exactly one CLI argument as testcase file name with extension \n"); 
		exit(0); 
	}
    
    FILE *fptr; //file object 
	
    //open file in read mode
	fptr = fopen(argv[1], "r"); 
	
    //check if file found or not
    if (fptr == NULL){ 
		printf("Cannot open file \n"); 
		exit(0); 
	} 
	
    int userCount;
    char input[8005];   // line of file at a time
    int len;

    //to get first line (number of users ->count)
    if(fscanf(fptr, "%s", input) != EOF){
        int len = strlen(input);
        //printf("User:%s\n",input);
        for(int i=0;i<len;i++){
            int num=0;
            while(i<len && input[i]!=','){
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            userCount = num;
        }  
    }
    
    int paths[userCount][1001]; //  paths each could be of 1000 nodes
    int pathLen[userCount]; 

    // Sub Path cant be larger than the smallest Path,
    // So we do our search from length 1 to length "smallestPathLen" only 
    int smallestPathLen=2001;

    // Largest Vertex Id is needed for building the hash function
    int largestVerticesId = -1; //largestVerticesId

    preprocessingInputFile(fptr,paths,pathLen,&largestVerticesId,&smallestPathLen);
    
    int status;
    int flag=0;

    int low=1;
    int high = smallestPathLen;
    
    int maximulSubpathLen, maximulSubpathno;
    
    //to store the endpoints of the maximal Common SubPath
    struct Node* tmpNode = (struct Node*)(malloc(sizeof(struct Node)));
    tmpNode->next = NULL;
    tmpNode->sId = 0;	
    tmpNode->eId = -1;

    //All l length substring check using Binary Search
    while (low <= high) {
        int mid = low + (high - low) / 2; 

        calculateRollingHashAtOnce(ht, paths, pathLen[0], 0, mid, userCount, tmpNode, largestVerticesId);
        flag=0;
        
        //for other users (2nd user)
        for(int idx=1;idx<userCount;idx++){

            status = calculateRollingHashAtOnce(ht, paths, pathLen[idx], idx, mid, userCount, tmpNode, largestVerticesId);
                
                if(status==2){// Got the answer
                    flag = 1;
                    maximulSubpathLen = mid;
                    maximulSubpathno = idx;
                    break;
                }
                else if(status!=1){ //Change the length
                    break;
                }

        }
        
        // if found, check for next bigger l than current
        // else check for lower l than current
        if(flag)
            low = mid + 1;
        else
            high = mid - 1;

    } 
  

    printf("MaximalCommonSubPath: ");
    int tmpSid = tmpNode->sId;
    int tmpEid = tmpNode->eId;

    for(int qw = tmpSid; qw <= tmpEid; qw++){
        printf("%d",paths[maximulSubpathno][qw]);
        if(qw!=tmpEid){
            printf(",");
        }
    }
    printf("\n");

    free(ht);
    free(tmpNode);
	return 0; 
	
}