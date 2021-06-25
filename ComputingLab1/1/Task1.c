/*
Assignment No. : 1
Task :           1
*/


#include <stdio.h> 
#include <stdlib.h> // For exit() 
#include <string.h>
#include <time.h>

/*
    maximalCommonSubpath()  - general Working Of Maximal Common Substring

    path1[]- path of user 1
    path2[]- path of user 2
    path1Len- length of user 1
    path2Len- length of user 2

*/
void maximalCommonSubpath(int path1[], int path2[], int path1Len, int path2Len){
    int dp[path1Len+1][path2Len+1];
    
    for(int i=0;i<path1Len;i++){
        dp[i][0]=0;
    }

    for(int i=0;i<path2Len;i++){
        dp[0][i]=0;
    }

    int maximalLen=0;
    int lastRow=0,lastCol=0;

    for(int i=0;i<=path1Len;i++){
        for(int j=0;j<=path2Len;j++){
            dp[i][j] = 0;
            if(i!=0 && j!=0 && path1[i-1] == path2[j-1]){
                dp[i][j] = dp[i-1][j-1] + 1;
                if(dp[i][j]>maximalLen){
                    maximalLen = dp[i][j];
                    lastRow = i;
                    lastCol = j;
                }
            }
        }
    }
    printf("MaximalCommonSubPath: ");
    if(maximalLen){
        int i=0;
        //going diagonally to print out the MaximalCommonSubPath
        while(i<maximalLen){
            printf("%d",path1[lastRow-maximalLen+i]);
            if(i!=maximalLen-1)
                printf(",");
            i++;
        }
        printf("\n");
    }
}


/*
    preprocessingInputFile() -  Preprocess Input File to extract paths into arrays
    fptr                     -  File Pointer
    paths                    -  Array to store paths
    pathLen                  -  Array to store path lengths

*/
void preprocessingInputFile(FILE* fptr, int paths[][1001], int pathLen[]){
    char input[8005];   // array for line wise reading of file
    int j=0,len,k=0;
    
    // File preprocessing
    while(fscanf(fptr, "%s", input) != EOF){
        int len = strlen(input);
        k=0;
        
        //to remove values before ":"
        int qw=0;
        while(input[qw]!=':'){
            qw++;
        }

        for(int i=qw+1;i<len;i++){
            if(input[i]==',')
                continue;

            int num=0;
            //Converting String into Number
            while(i<len && input[i]!=','){
                num = num*10 + ((int)(input[i]-'0'));
                i++;
            }
            paths[j][k++] = num;
        }
        pathLen[j]=k;    
        j++;   
    }
    
}

int main(int argc, char** argv){

    /*time check
    clock_t t; 
    t = clock();
    */
    
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
	
	int userCount = 2;
    int paths[userCount][1001]; //  paths, each could be of 1000 nodes
    int pathLen[userCount]; //store path length of each path
    
    // Preprocess File to extract paths into arrays
    preprocessingInputFile(fptr, paths, pathLen);
    fclose(fptr); 
	    
    //finding MaximalCommonSubpath
    maximalCommonSubpath(paths[0], paths[1], pathLen[0], pathLen[1]);
    
	return 0; 

}
