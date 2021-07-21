/*
    Name : Vipray Jain 
    Assignment No: 7
    Implementation Type : 3
*/


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*   
The following structure contains the necessary information  
to allow the function "matprod" to access its input data and 
place its output into the structure.  
*/

// compute A[i,:] * B[:,j] and store in C[i,j]
typedef struct _thread_data {
    double **A;
    double **B;
    double **C;
    int r1,c1,r2,c2; //should be c1=r2
} ThreadData;


/* Define globally accessible variables and a mutex */
#define NUMTHRDS 4
ThreadData matData; 
pthread_t callThd[NUMTHRDS];
pthread_mutex_t gid_mutex, fTC_mutex;
pthread_cond_t count_threshold_cv;
int global_Id, freeThreadCounter;


/*  
    matprod() - funtion to compute (i,j) element of matrix C using Threads
    arguments   -   thread Id

*/
void *matprod(void *arguments)
{
    // Thread ID
    int id = (int)arguments;
        
    while(1){
        int im,jm;
        
        pthread_mutex_lock (&gid_mutex);
            // Increase counter to let main() know about it
            pthread_mutex_lock(&fTC_mutex);
                freeThreadCounter++;
            pthread_mutex_unlock(&fTC_mutex);
            
            // Wait for main()'s signal to take next job in hand
            pthread_cond_wait(&count_threshold_cv, &gid_mutex);
            
            // check if all elements are already calculated
            if(global_Id == (matData.r1 * matData.c2)){
                // die, if yes
                pthread_mutex_unlock (&gid_mutex);
                printf("matprod() thread %d: Going to Die...\n",id);
                break;
            }
        
            // global_id is used to get value of i,j for current thread.
            im = global_Id / matData.c2;
            jm = global_Id % matData.c2;
            global_Id++;
        pthread_mutex_unlock (&gid_mutex);
        int r = rand()%10 +1;
        r = r/10;
        sleep(r);

        // no lock is required for computaion and assignment as 
        // element(i,j) will only be assigned to any one thread.
        double tmp = 0;
        for(int p=0;p<matData.c1;p++){
            tmp += matData.A[im][p] * matData.B[p][jm];
        }
        
        matData.C[im][jm] = tmp;
        printf("matprod() thread %d: computed (%d,%d)= %lf \n",id,im,jm,tmp);
        
        /* OLD CODE position for incrementing the freeThreadCounter
        pthread_mutex_lock(&fTC_mutex);
            freeThreadCounter++;
            printf("matprod() thread %d: freeThreadCounter %d; unlocking ftc_mutex...\n",id,freeThreadCounter);
        pthread_mutex_unlock(&fTC_mutex);
        sleep(1);
        */
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
    //char input[8005];   // one line of file at a time
    char *input = (char *)(malloc)(8005*sizeof(char)); 
    char* tok;

    //Matrix B
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


    global_Id = 0;
    freeThreadCounter = 0;
    void *status;
    pthread_attr_t attr;  

    // Create threads to perform the dotproduct 
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_cond_init (&count_threshold_cv, NULL);
    pthread_mutex_init(&gid_mutex, NULL);

    /*
        Each thread accesss the global_id
        Compute i,j from that global_id
        then increment global_id
        then wait for signal from main() thread
        and then repeate the same process for different value of global_id

        Using This way, only NUMTHRDS threads are able to compute all the elememts.
    */
    int idNo=0;
    for(int i=0;i<NUMTHRDS;i++){
        pthread_create(&callThd[idNo], &attr, matprod, (void *)idNo);
        idNo++;
    }
    
    // To make sure that threads are actually waiting for the singnal
    sleep(5);
    
    // find available thread to compute elements of Matrix C
    for(int i=0;i<matData.r1;i++){
        for(int j=0;j<matData.c2;j++){
            //Send the signal
            /*
                'fTC_mutex' is used to access shared variable 'freeThreadCountefreeThreadCounterr'
                'freeThreadCounter' is used to keep the count of free threads, 
                waiting for main() thread's signal
            */
            
            /*
                Someone might think that 'while(freeThreadCounter<=0);' is a crical section
                and should be access with proper isolation.
                But In this problem, it is not create any problem whatsoever.
                Because all the other threads are increasing the value.
                So it never be the case that while loop terminates and 
                on reaching 'freeThreadCounter--;' , the value became <=0.
                It is so because, main() is the only thread that is decreasing the value.
                Thus its fine to keep it outside the mutex.

                If you want to do it the Right way
                then instead of only one, use two mutex for 'freeThreadCounter'
                one for read and one for write.
            */
            while(freeThreadCounter<=0);
            pthread_mutex_lock(&fTC_mutex);
                freeThreadCounter--;
            //    printf("main(): freeThreadCounter %d; singnalling Condition Variable\n",freeThreadCounter);
            pthread_mutex_unlock(&fTC_mutex);
            pthread_cond_signal(&count_threshold_cv);
            
            // random sleep time
            int r = rand()%10 +1;
            r = r/10;
            sleep(r);    
        }
    }

    // when all elements got computed, final signal sent to every Thread so they can die.
    for(int i=0;i<NUMTHRDS;i++){
        //Send the signal
            while(freeThreadCounter<=0);
            pthread_mutex_lock(&fTC_mutex);
                freeThreadCounter--;
            //    printf("main(): freeThreadCounter %d; singnalling Condition Variable\n",freeThreadCounter);
            pthread_mutex_unlock(&fTC_mutex);
            pthread_cond_signal(&count_threshold_cv);
            // random sleep time
            int r = rand()%10 +1;
            r = r/10;
            sleep(r);
    }
    
    
    // Wait on the other threads
    idNo=0;
    for(int i=0;i<NUMTHRDS;i++){
        pthread_join(callThd[idNo], &status);
        idNo++;
    }

    // After joining, print out the results and cleanup 
    printf("\n\n------C-----\n\n");
    for(int i=0;i<matData.r1;i++){
        for(int j=0;j<matData.c2;j++){
            printf ("%lf ", matData.C[i][j]);
        }
        printf("\n");
    }

    // Destruction Area
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&gid_mutex);

    free(matData.A);
    free(matData.B);
    free(matData.C);

    pthread_exit(NULL);

    return 0;
}   
