/*
    Name : Vipray Jain 
    Assignment No: 7
    Implementation Type : 1
*/


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// compute A[i,:] * B[:,j] and store in C[i,j]
typedef struct _thread_data {
    double **A;
    double **B;
    double **C;
    int r1,c1,r2,c2; //should be c1=r2
} ThreadData;



/* Define globally accessible variables */

ThreadData matData; 
pthread_t *callThd;


/*  
    matprod() - funtion to perform multiplication using threads
    input   -   string
*/
void *matprod(void *arguments)
{

    int id = (int)arguments;
    int im = id / matData.c2;
    int jm = id % matData.c2;
    matData.C[im][jm] = 0;
    /*
        Here, we do not need Mutual Exclusion because
        we are creating r1*c2 threads. So each thread will write at its own assigned place
        so there is no race condition possible.
    */
    for(int p=0;p<matData.c1;p++){
        matData.C[im][jm] += matData.A[im][p] * matData.B[p][jm];
    }

    pthread_exit((void*) 0);

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
    parseInput() - funtion to parse file into graph
    fptr   -   file pointer
*/
void parseInput(FILE *fptr){
    char *input = (char *)(malloc)(8005*sizeof(char)); 
    char* tok;

    //Matrix A
    if((fgets (input, 8000, fptr)!=NULL) && (!isEmptyLine(input))){

        tok = strtok(input, " ");
        matData.r1 = atoi(tok);

        tok = strtok(0, " ");
        matData.c1 = atoi(tok);

    }

    //memory Intiallization
    matData.A = (double **)(malloc)((matData.r1)*sizeof(double *));
    for(int iq=0;iq<matData.r1;iq++){
        matData.A[iq] = (double *)(malloc)((matData.c1)*sizeof(double));
    }



    int cnt=0;
    while(cnt<matData.r1 && (fgets (input, 8000, fptr)!=NULL)){
        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }
        tok = strtok(input, " ");
        for(int j=0;j<matData.c1;j++){
            matData.A[cnt][j] = atof(tok);
            tok = strtok(0, " ");
        }
        cnt++;  
    }
    
    

    //Matrix B
    while((fgets (input, 8000, fptr)!=NULL)){

        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }
        
        tok = strtok(input, " ");
        matData.r2 = atoi(tok);

        tok = strtok(0, " ");
        matData.c2 = atoi(tok);
        break;
    }



    //memory Intiallization
    matData.B = (double **)(malloc)((matData.r2)*sizeof(double *));
    for(int iq=0;iq<matData.r2;iq++){
        matData.B[iq] = (double *)(malloc)((matData.c2)*sizeof(double));
    }


    cnt=0;
    while(cnt<matData.r2 && (fgets (input, 8000, fptr)!=NULL)){
        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }
        tok = strtok(input, " ");
        for(int j=0;j<matData.c2;j++){
            matData.B[cnt][j] = atof(tok);
            tok = strtok(0, " ");
        }
        cnt++;  
    }

    //memory Intiallization
    matData.C = (double **)(malloc)((matData.r1)*sizeof(double *));
    for(int iq=0;iq<matData.r1;iq++){
        matData.C[iq] = (double *)(malloc)((matData.c2)*sizeof(double));
    }
    
    free(input);
}





int main (int argc, char *argv[])
{

    if (argc != 2){ 
		printf("Please pass Exactly 1 CLI argument as testcase file \n"); 
		exit(0); 
	}

    FILE *fptr; //file object
    fptr = fopen(argv[1], "r"); 
    if (fptr == NULL){ 
        printf("File Not Found\n");    
        exit(0); 
    }

    parseInput(fptr);
    fclose(fptr);
            
    printf("\n\n-----A------\n\n");
    for(int i=0;i<matData.r1;i++){
        for(int j=0;j<matData.c1;j++){
            printf ("%lf ", matData.A[i][j]);
        }
        printf("\n");
    }    

    printf("\n\n------B-----\n\n");
    for(int i=0;i<matData.r2;i++){
        for(int j=0;j<matData.c2;j++){
            printf ("%lf ", matData.B[i][j]);
        }
        printf("\n");
    }


    void *status;
    pthread_attr_t attr;  

    callThd = (pthread_t *)(malloc)((matData.r1*matData.c2)*sizeof(pthread_t));
        
    // Create threads to perform the dotproduct 
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    
    int idNo=0;
    for(int i=0;i<matData.r1;i++){
        for(int j=0;j<matData.c2;j++){
            pthread_create(&callThd[idNo], &attr, matprod, (void *)idNo);
            idNo++;
        }
    }
    
    pthread_attr_destroy(&attr);

    // Wait on the other threads
    idNo=0;
    for(int i=0;i<matData.r1;i++){
        for(int j=0;j<matData.c2;j++){
            pthread_join(callThd[idNo], &status);
            idNo++;
        }
    }

    // After joining, print out the results and cleanup 
    printf("\n\n------C-----\n\n");
    for(int i=0;i<matData.r1;i++){
        for(int j=0;j<matData.c2;j++){
            printf ("%lf ", matData.C[i][j]);
        }
        printf("\n");
    }

    free(matData.A);
    free(matData.B);
    free(matData.C);
    free(callThd);

    pthread_exit(NULL);

    return 0;
}   
