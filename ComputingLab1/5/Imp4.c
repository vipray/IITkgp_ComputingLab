/*
    Name : Vipray Jain 
    Roll : 20CS60R37
    Assignment No: 7
    Implementation Type : 3
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NUMTHRDS 4
#define BufferSize 4

pthread_cond_t empty;
pthread_cond_t notEmpty;
pthread_mutex_t mutex;

int *buffer;
int jobs_in_buffer=0;
int count=0;
int count1=0;
int len=0;

/*   
The following structure contains the necessary information  
to allow the function "dotprod" to access its input data and 
place its output into the structure.  
*/

// compute A[i,:] * B[:,j] and store in C[i,j]
typedef struct _thread_data {
    double **A;
    double **B;
    double **C;
    int r1,c1,r2,c2; //should be c1=r2
} ThreadData;


ThreadData matData;

/*  
    notEmptyLine() - funtion to check whther line is empty or not
    input   -   string

*/
int notEmptyLine(const char *input)
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
    fptrtr   -   file pointer
*/
void parseInput(FILE *fptrtr){
    //char input[8005];   // one line of file at a time
    char *input = (char *)(malloc)(8005*sizeof(char)); 
    char* tok;

    //Matrix B
    if((fgets (input, 8000, fptrtr)!=NULL) && (!notEmptyLine(input))){

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
    while(cnt<matData.r1 && (fgets (input, 8000, fptrtr)!=NULL)){
        if(notEmptyLine(input)){ //remove empty lines
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
    while((fgets (input, 8000, fptrtr)!=NULL)){

        if(notEmptyLine(input)){ //remove empty lines
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
    while(cnt<matData.r2 && (fgets (input, 8000, fptrtr)!=NULL)){
        if(notEmptyLine(input)){ //remove empty lines
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

void* consumer(void *arg)
{   
    while(1){           
        pthread_mutex_lock(&mutex);
        int tmp=0;

        if(count1<=0){
            // if all elements are computed
            // then die
            //pthread_cond_signal(&notEmpty);
            pthread_mutex_unlock(&mutex);
            break;
        }
    
        count1--;
        while(len == 0){         
            // if buffer is empty
            // wait till producer produce  
            pthread_cond_wait(&notEmpty, &mutex);
        }     
        
        len-=1;        
        tmp=buffer[len];   

        // After every consumption, let producer know to produce more    
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        if(tmp<0)
            break;

        // computing the element
        int row = (tmp / matData.c2) ;
        int col = (tmp % matData.c2);
        for (int i = 0; i < matData.c1; i++){        
            matData.C[row][col] += matData.A[row][i] * matData.B[i][col];
        }
    }
    pthread_exit(NULL);
}

void* producer(void *arg) 
{   
	while(1)
    {
        pthread_mutex_lock(&mutex);
        while(len == BufferSize){
            // If buffer is full, let consumer know about it
            // Note that the pthread_cond_wait routine
            // will automatically and atomically unlock mutex while it waits.           
            pthread_cond_wait(&empty, &mutex);
        }

        if(count<=0){
            // if all elements are computed
            // let the consumers know about it
            // and leave
            pthread_cond_signal(&notEmpty);
            pthread_mutex_unlock(&mutex);
            break;
        }

        count--;     
      	buffer[len] = count;
        ++len; 

        // Ask consumer to consume after producing every product.       
        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL; 
   
}




int main(int argc, char *argv[])
{
    if (argc != 2){ 
		printf("Please pass Exactly 1 CLI argument as testcase file \n"); 
		exit(0); 
	}

    clock_t start, end;
    double cpu_time_used;

    start = clock();

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



    pthread_attr_t attr;
    pthread_t threads[NUMTHRDS];
    void *status;

    // shared buffer between producer and consumer
    buffer=(int *)malloc(BufferSize*sizeof(int));
	for(int i=0;i<BufferSize;i++){
    	buffer[i]=0;
	}

    count = matData.r1*matData.c2;
    count1 = count;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    /*
        *   All threads will start as consumer.
        *   main() thread will start as only producer.
        *   main() will produce in buffer and let the consumer know about it using cond varaible.
        *   all other threads(consumers) will consume id from buffer, computer corresponding element of Matrix C and then let producer know that i have consumed.

    */
    for(int i = 0; i < NUMTHRDS; ++i){
		pthread_create(&threads[i], &attr, consumer, (void *)&i);
	}
    
    int p=0;
    producer((void *)&p);

    
	for (int i = 0; i < NUMTHRDS; ++i){        
		pthread_join(threads[i],NULL);
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
    pthread_mutex_destroy(&mutex);

    free(matData.A);
    free(matData.B);
    free(matData.C);
    free(buffer);
    
    pthread_exit(NULL);

    return 0;

}