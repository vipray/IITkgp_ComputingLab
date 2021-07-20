/*
Name:            Vipray Jain
Assignment No. : 6
Task :           1
*/


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>    /* Include this to use shared mem */
#include <sys/sem.h>	/* Include this to use semaphores */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h> // library for fcntl function
#include <string.h>

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing the V(s) operation */
#define TRAIN_COUNT 3
#define PROCESS_COUNT 4 


/*  
    struct reservation -   To store the details of the passenger
    
*/
struct reservation{
    int pnr;
    char passengerName[20], age[3], sex;
    char class[4]; // AC2, AC3, or SC
    int status; // waitlisted or confirmed
};




/*  
    struct train -   to store Train wise info

*/
struct train{
    int trainId;
    int AC2, AC3, SC; // No. of available berths
    int totalbooked;
    struct reservation rlist[1000];//struct reservation *rlist;
};



/*  
    struct requests -   temporary stores the data parse from the line of file

*/
struct requests{
    int type;   //0 resrve, 1 cancell
    int pnr;
    int trainId;
    char passengerName[20], age[3], sex;
    char class[4]; // AC2, AC3, or SC
};


/*  
    Global Variables So that make funtions for get and release MUTEXs

*/
int shmTrains,shmReaderCount,shmWriterCount;  //shared memory
int *counterRead, *counterWrite;    // shared Counters
int i;

int mutexWrite[TRAIN_COUNT], mutexRead[TRAIN_COUNT];    // read and Write mutex for each train
int x[TRAIN_COUNT]; // mutex for counterRead array
int y[TRAIN_COUNT]; // mutex for counterWrite array
int z[TRAIN_COUNT]; // mutex to ensure writer gets priority over multiple waiting readers
int printLock ;     // mutex to lock while printing on the terminal

struct train *rd;   // shared Trains structure
struct sembuf pop, vop ;    // attributes for signal() and wait() functions




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
    parseLine() -   function to parse line of file
    input   -   line content
    req -   structure to store file data in structural and understanding way

*/
void parseLine(char *input, struct requests* req){
    char* tok;
    int c;
        
    tok = strtok(input, " ");
    
    if(tok[0] == 'r' || tok[0] == 'R' ){
        //type
        (*req).type = 0;

        //name1
        tok = strtok(0, " ");
        strcpy((*req).passengerName, tok);
        
        
        strcat((*req).passengerName, " ");
        //name2
        tok = strtok(0, " ");
        strcat((*req).passengerName, tok);
        
        
        //Age
        tok = strtok(0, " ");
        //c = atoi(tok);
        strcpy((*req).age, tok);
        
        //sex
        tok = strtok(0, " ");
        (*req).sex = tok[0];

        //TrainId
        tok = strtok(0, " ");
        c = atoi(tok);
        (*req).trainId = c;
        
        //Class
        tok = strtok(0, " ");
        strcpy((*req).class, tok);
        if((*req).class[2]=='2'||(*req).class[2]=='3'){
            (*req).class[3]='\0';
        }
        else{
            (*req).class[2]='\0';
        }
    }
    else{
        (*req).type = 1;

        //pnrId
        tok = strtok(0, " ");
        c = atoi(tok);
        (*req).pnr = c;
    }
}



/*
//    lineCount() -   function tocount number of lines in the file
//    s   -   file name



int lineCount(char *s){
    FILE *fptr; //file object 
	
    //open file in read mode
	fptr = fopen(s, "r");
    if (fptr == NULL){ 
		
		exit(0); 
	} 

    char *input = (char*)malloc(100*sizeof(char));   
    int noOf=0;//Count edges
    while(fgets (input, 100, fptr)!=NULL){
        if(isEmptyLine(input)){ //remove empty lines
            continue;
        }
        noOf++;
    }
    fclose(fptr);
    free(input);
    return noOf;
}
*/



/*
    getReadLock() -   Get the read lock on perticular Train
    Look: http://faculty.juniata.edu/rhodes/os/ch5d.htm (Semaphore Solution: Writers have Priority)
*/
void getReadLock(int tid){
    P(z[tid]);
    P(mutexRead[tid]);
    P(x[tid]);

    counterRead[tid]++;
    if(counterRead[tid] == 1){
        P(mutexWrite[tid]);
    }

    V(x[tid]);
    V(mutexRead[tid]);
    V(z[tid]);        
}




/*
    releaseReadLock() -   Release the read lock on perticular Train

*/
void releaseReadLock(int tid){
    P(x[tid]);

    counterRead[tid]--;
    if(counterRead[tid] == 0){
        V(mutexWrite[tid]);
    }
    
    V(x[tid]);         
}



/*
    getWriteLock() -   Get the right lock on perticular Train

    For a perticular train, As first writer comes into the picture, it sets mutexRead
    which prevent any more new readers from entring into the critical section 
    irrespective of wether some readers are already in CS or not 
*/
void getWriteLock(int tid){
    P(y[tid]);

    counterWrite[tid]++;
    if (counterWrite[tid] == 1){ 
        P(mutexRead[tid]);
    }
    
    V(y[tid]); 
    P(mutexWrite[tid]);
            
}



/*
    releaseWriteLock() -   Relase the right lock on perticular Train

*/

void releaseWriteLock(int tid){
    V(mutexWrite[tid]);
    P(y[tid]);

    counterWrite[tid]--;
    if (counterWrite[tid] == 0)
        V(mutexRead[tid]);
    
    V(y[tid]);

}



/*
    releaseSemaphores() -   To release Semaphores

*/

void releaseSemaphores(){
    /* Semaphores */
    for(int i=0;i<TRAIN_COUNT;i++){
        semctl(mutexWrite[i], 0, IPC_RMID, 1);
        semctl(mutexRead[i], 0, IPC_RMID, 1);
        semctl(x[i], 0, IPC_RMID, 1);
        semctl(y[i], 0, IPC_RMID, 1);
        semctl(z[i], 0, IPC_RMID, 1);
    }
    semctl(printLock, 0, IPC_RMID, 1);
}


/*
    releaseSharedMemory() -   To release Shared Memory

*/

void releaseSharedMemory(){
    shmctl(shmTrains,IPC_RMID,NULL);
    shmctl(shmReaderCount,IPC_RMID,NULL);
    shmctl(shmWriterCount,IPC_RMID,NULL);  
}



/*
    getSharedMemory() -   To get memory Shared

*/
void getSharedMemory(){
		/* We request an array of 3 struct train */
    shmTrains = shmget(IPC_PRIVATE, TRAIN_COUNT*sizeof(struct train), 0777|IPC_CREAT);
    shmReaderCount = shmget(IPC_PRIVATE, TRAIN_COUNT*sizeof(int), 0777|IPC_CREAT);
    shmWriterCount = shmget(IPC_PRIVATE, TRAIN_COUNT*sizeof(int), 0777|IPC_CREAT);
}




/*
    getSemaphores() -   To intiallize Semaphores

*/
void getSemaphores(){
    /* Semaphores */
    
    for(int i=0;i<TRAIN_COUNT;i++){
        mutexWrite[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        mutexRead[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        x[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        y[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        z[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
    
        semctl(mutexWrite[i], 0, SETVAL, 1);
        semctl(mutexRead[i], 0, SETVAL, 1);
        semctl(x[i], 0, SETVAL, 1);
        semctl(y[i], 0, SETVAL, 1);
        semctl(z[i], 0, SETVAL, 1);
    }
    printLock = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
    semctl(printLock, 0, SETVAL, 1);

}





/*
    detachSharedMemory() -   detach shared memory from the process

*/

void detachSharedMemory(){
    shmdt(rd);
    shmdt(counterRead);
    shmdt(counterWrite);
}




int main(int argc, char** argv){
    
    if (argc != 5){ 
		printf("Please pass Exactly 4 CLI argument as testcase file \n"); 
		exit(0); 
	}

    getSharedMemory();
	
    getSemaphores();

	pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = SEM_UNDO;
	pop.sem_op = -1 ; vop.sem_op = 1 ;


    int cc = PROCESS_COUNT;
    int c2 = cc;
	int pids[cc];
    int myPid=909;
    int tag=100;

    // assign shared memory to counters and intiallize them to 0
    counterRead = (int *) shmat(shmReaderCount, 0, 0);
    counterWrite = (int *) shmat(shmWriterCount, 0, 0);
    
    for(int i=0;i<TRAIN_COUNT;i++){
        counterRead[i] = 0;//readerCounter
        counterWrite[i] = 0;//writerCounter
    }

    // Assign shared memory for Trains
    // Intiallize them with initiall attributes
    rd = (struct train *) shmat(shmTrains, 0, 0);
    
    for(int i=0;i<TRAIN_COUNT;i++){
        rd[i].AC2 = 10;
        rd[i].AC3 = 10;
        rd[i].SC = 10;
        rd[i].totalbooked = 0;
        rd[i].trainId = i;
    }

    //Child Creation
    while(c2--){
        if((pids[c2] = fork()) == 0){

			/* child process */
            myPid = 0;
            tag = c2;
            rd = (struct train *) shmat(shmTrains, 0, 0);
            counterRead = (int *) shmat(shmReaderCount, 0, 0);
            counterWrite = (int *) shmat(shmWriterCount, 0, 0);
            break;
		}  
	}

    //Child
    if(myPid == 0){
        sleep(1);
        
        FILE *fptr; //file object 
        char input[100];   
        
        //open assigned file in read mode
        fptr = fopen(argv[tag+1], "r"); 
        if (fptr == NULL){ 
            
            exit(0); 
        }

        while(fgets (input, 100, fptr)!=NULL){
            if(isEmptyLine(input)){ //remove empty lines
                continue;
            }
            
            struct requests req;
            parseLine(input,&req);

            int tid;
            if(req.type == 0){   //reserve
                tid = req.trainId;
            }
            else{//Cancell
                int pnrId = req.pnr;
                tid = pnrId % 10;
            }
            
        //----------------------Reader-----------------------------------
            getReadLock(tid);
        
            if(req.type == 0){  //reserve
                
                if(req.class[0] == 'S'){
                    
                    P(printLock);
                    printf("\n%d: Availability('-' represent waiting) in Train %d and Class SC is => %d\n",tag+1,tid,rd[tid].SC);
                    V(printLock);
                }
                else if(req.class[2] == '2'){
                    P(printLock);
                    printf("\n%d: Availability('-' represent waiting) in Train %d and Class AC2 is => %d\n",tag+1,tid,rd[tid].AC2);
                    V(printLock);
                }
                else if(req.class[2] == '3'){
                    P(printLock);
                    printf("\n%d: Availability('-' represent waiting) in Train %d and Class AC3 is => %d\n",tag+1,tid,rd[tid].AC3);
                    V(printLock);
                }
            }
            else{ //Cancell
                int pnrId = req.pnr;
                P(printLock);
                printf("\n%d: Ticket With PNR %d is to be cancelled\n",tag+1,pnrId);
                V(printLock);
            }
            
            releaseReadLock(tid);
            
        //-------------------------Reader--------------------------------
            
        //-------------------------Writer--------------------------------
            
            getWriteLock(tid);
            
            
            if(req.type == 0){   //reserve
                int idxe = rd[tid].totalbooked;
                
                if(req.class[0] == 'S'){
                    rd[tid].SC--;
                    if(rd[tid].SC<0){
                        rd[tid].rlist[idxe].status = 0; //waiting
                    }
                    else{
                        rd[tid].rlist[idxe].status = 1; //Reserved
                    }
                }
                else if(req.class[2] == '2'){
                    rd[tid].AC2--;
                    if(rd[tid].AC2<0){
                        rd[tid].rlist[idxe].status = 0; //waiting
                    }
                    else{
                        rd[tid].rlist[idxe].status = 1; //Reserved
                    }
                }
                else if(req.class[2] == '3'){
                    rd[tid].AC3--;
                    if(rd[tid].AC3<0){
                        rd[tid].rlist[idxe].status = 0; //waiting
                    }
                    else{
                        rd[tid].rlist[idxe].status = 1; //Reserve
                    }
                }
                strcpy(rd[tid].rlist[idxe].age, req.age);
                strcpy(rd[tid].rlist[idxe].class, req.class);
                strcpy(rd[tid].rlist[idxe].passengerName, req.passengerName);
                rd[tid].rlist[idxe].sex = req.sex;
                
                char line[10];
                sprintf(line,"%d%d\n",idxe,tid);    //pnr = "seatNo"."trainId"
                rd[tid].rlist[idxe].pnr = atoi(line);
                
                rd[tid].totalbooked++;

                if(rd[tid].rlist[idxe].status){
                    P(printLock);
                    printf("\n%d: Reservation made successfully for passenger %s in process %d, allocated PNR %d\n",tag+1,rd[tid].rlist[idxe].passengerName,tag+1,rd[tid].rlist[idxe].pnr);
                    V(printLock);
                }
                else{
                    P(printLock);
                    printf("\n%d: Added passenger %s to waitlist by process %d\n",tag+1,rd[tid].rlist[idxe].passengerName,tag+1);
                    V(printLock);
                }
                    
            }
            else{//Cancell
                int pnrId = req.pnr;
                int seatNo = pnrId / 10;

                // check if its a valid ticket
                if(seatNo < rd[tid].totalbooked && rd[tid].rlist[seatNo].status!=2){
                    // if a reserved ticket is cancelled, reserve first waiting ticket(if any)
                    if(rd[tid].rlist[seatNo].status == 1){

                        char classs[4];
                        strcpy(classs, rd[tid].rlist[seatNo].class);
                        classs[3] ='\0';
                        
                        int flag=0; //is there anyone in waiting

                        if(classs[0] == 'S'){
                            rd[tid].SC++;
                            if(rd[tid].SC<=0){
                                flag =1;
                            }
                            classs[2] ='\0';
                        }
                        else if(classs[2] == '2'){
                            rd[tid].AC2++;
                            if(rd[tid].AC2<=0){
                                flag =1;
                            }
                        }
                        else if(classs[2] == '3'){
                            rd[tid].AC3++;
                            if(rd[tid].AC3<=0){
                                flag =1;
                            }
                        }

                        //If someOne is in waiting then reserve first one's ticket
                        for(int it=seatNo+1;(it<rd[tid].totalbooked && flag);it++){
                            if(strcmp(rd[tid].rlist[it].class,classs) == 0 && rd[tid].rlist[it].status == 0){
                                rd[tid].rlist[it].status = 1;
                                P(printLock);
                                printf("\n%d: Removed passenger %s from waitlist by process %d, allocated PNR %d\n",tag+1,rd[tid].rlist[it].passengerName,tag+1,rd[tid].rlist[it].pnr);
                                V(printLock);
                                
                                break;
                            }
                        }
                    }
                    
                    //set cancelled ticket status as 2
                    rd[tid].rlist[seatNo].status = 2;
                    
                    P(printLock);
                    printf("\n%d: Reservation Cancelled PNR %d for passenger %s By process %d\n",tag+1,rd[tid].rlist[seatNo].pnr, rd[tid].rlist[seatNo].passengerName,tag+1);
                    V(printLock);
                    
                }
                else{
                    P(printLock);
                    printf("\n%d: No valid ticket with PNR %d is found\n",tag+1,pnrId);
                    V(printLock);  
                }
            }
            
            releaseWriteLock(tid);
        //write ends

        //Writer---------------------------------------------------------
            //sleep(1);

        }
        fclose(fptr);
        P(printLock);
        printf("Child %d is terminating\n",tag+1);
        V(printLock);
        
        detachSharedMemory();
        sleep(5);
        exit(0);
    }

    //parent
    if(myPid!=0){
        int kl;
        //wait for all children to terminate
        while ((kl = wait(NULL)) > 0);
        
        detachSharedMemory();
        releaseSemaphores();
        releaseSharedMemory();
        
        printf("Parent %d is terminating\n",getpid());
	    exit(0);
    }
    
}
