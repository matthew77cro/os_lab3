#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MIN_SLEEP 1 
#define MAX_SLEEP 5
#define RIDE_DURATION 7

int moj_pid;

int SemId;   /* identifikacijski broj skupa semafora */

void SemGet(int n)
{  /* dobavi skup semafora sa ukupno n semafora */
   SemId = semget(IPC_PRIVATE, n, 0600);
   if (SemId == -1) {
      printf("Nema semafora!\n");
      exit(1);
   }
}

int SemSetVal(int SemNum, int SemVal)
{  /* postavi vrijednost semafora SemNum na SemVal */
   return semctl(SemId, SemNum, SETVAL, SemVal);
}

int SemOp(int SemNum, int SemOp)
{  /* obavi operaciju SemOp sa semaforom SemNum */
   struct sembuf SemBuf;
   SemBuf.sem_num = SemNum;
   SemBuf.sem_op  = SemOp;
   SemBuf.sem_flg = 0;
   return semop(SemId, & SemBuf, 1);
}

void SemRemove(void)
{  /* uništi skup semafora */
   (void) semctl(SemId, 0, IPC_RMID, 0);
}

void exit_all(int signal){
    
    SemRemove();
    exit(0);
    
}

void posjetitelj(){
    
    printf("%6d\n", moj_pid);
    SemOp(0,-1);
    SemOp(1,1);
    printf("         %6d\n", moj_pid);
    SemOp(2,-1);
    printf("                    %6d\n", moj_pid);
    SemOp(3,1);
    
}

void vrtuljak(int n){
    
    int i;
    while(1){
     
        for(i=0; i<n; i++)
            SemOp(0,1);
        for(i=0; i<n; i++)
            SemOp(1,-1);
        for(i=RIDE_DURATION; i>0; i--){
            printf("                              %dsec\n", i);
            sleep(1);
        }
        printf("                              KRAJ\n");
        for(i=0; i<n; i++){
            SemOp(2,1);
            sleep(1);
        }
        for(i=0; i<n; i++)
            SemOp(3,-1);
        
    }
    
}

int main(){
    
    moj_pid = getpid();
    sigset(SIGINT,exit_all);
    
    int n; //broj mjesta na vrtuljku
    printf("Unesite broj mogucih mjesta na vrtuljku: ");
    scanf("%d",&n);
   
   //Stvaranje semafora
   SemGet(4);
   
   //Stvaranje procesa vrtuljak
   switch(fork()){
        case -1:
            printf("Greška: ne mogu stvoriti proces vrtuljak\n");
            exit(1);
        case 0:
            moj_pid = getpid();
            vrtuljak(n);
            return 0;
        default:
            break;
    }
    
    srand(time(NULL));
    
    printf("STIGAO    P.SJEO    SILAZI    V.VOZI\n");
    printf("------------------------------------\n");
    
    int random_sleep;
    while(1){
        
        switch(fork()){
            case -1:
                printf("Greška: ne mogu stvoriti proces posjetitelj\n");
            case 0:
                moj_pid = getpid();
                posjetitelj();
                return 0;
            default:
                break;
        }
        
        random_sleep = rand()%(MAX_SLEEP-MIN_SLEEP+1)+MIN_SLEEP;
        sleep(random_sleep);
        
    }
    return 0;
    
}
