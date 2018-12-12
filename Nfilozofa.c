#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define MAX_RAND 5
#define MIN_RAND 1
#define RAND (rand()%(MAX_RAND-MIN_RAND+1)+MIN_RAND)

int nr_ciklusa = 0;

int n_filozofa = 0;
int n_stapica = 0;
int *n_ruku;

pthread_t *thr_id;
int *param;

pthread_mutex_t lock;
pthread_cond_t cond;

int *misle;
int *jedu;
int *cekaju;

void exit_all(){
 
    free(n_ruku);
    free(thr_id);
    free(param);
    free(misle);
    free(jedu);
    free(cekaju);
    pthread_mutex_destroy(&lock);
    exit(0);
    
}

void ucitaj_file(){
    
    FILE *f = fopen("./nfil_input.txt","r");
    fscanf(f,"%d",&n_stapica);
    fscanf(f,"%d",&n_filozofa);
    n_ruku = malloc(sizeof(int)*n_filozofa);
    for(int i=0; i<n_filozofa; i++){
     
        if(fscanf(f,"%d",n_ruku+i)==EOF){
            printf("Greška: nedovoljno ulaza u ulaznoj datoteci.");
            fclose(f);
            exit(1);
        }
        
    }
    fclose(f);
}

void m_uzmi_stapice(int i){
    pthread_mutex_lock(&lock);
    
        while(n_stapica<*(n_ruku+i)){
            cekaju[i]=1;
            pthread_cond_wait(&cond,&lock);
        }
        cekaju[i]=0;
        n_stapica-=*(n_ruku+i);
    
    pthread_mutex_unlock(&lock);
}

void m_spusti_stapice(int i){
    pthread_mutex_lock(&lock);
    
        n_stapica+=*(n_ruku+i);
        pthread_cond_broadcast(&cond);
    
    pthread_mutex_unlock(&lock);
}

void misli(int i){
    misle[i]=1;
    sleep(RAND);
    misle[i]=0;
}

void jedi(int i){
    jedu[i]=1;
    sleep(RAND);
    jedu[i]=0;
}

void filozof(int* i){
    
    int k=0;
    while(k<nr_ciklusa){
        
        //N.K.O.
        misli(*i);
        
        m_uzmi_stapice(*i);
        
        //K.O.
        jedi(*i);
        
        m_spusti_stapice(*i);
        
        k++;
        
    }
    
}

void ispis(){
    
    while(1){
        
        system("clear");
        printf("MISLITI: ");
        for(int i=0; i<n_filozofa; i++)
            if(misle[i]==1)
                printf("%d(%d) ",i,n_ruku[i]);
            else
                printf("     ");
        printf("\n");
            
        printf("CEKATI:  ");
        for(int i=0; i<n_filozofa; i++)
            if(cekaju[i]==1)
                printf("%d(%d) ",i,n_ruku[i]);
            else
                printf("     ");
        printf("\n");
        
        printf("JESTI:   ");
        for(int i=0; i<n_filozofa; i++)
            if(jedu[i]==1)
                printf("%d(%d) ",i,n_ruku[i]);
            else
                printf("     ");
        printf("\n");
        
        printf("STAPICA: %d\n\n",n_stapica);
        
        sleep(1);
        
    }
    
}

int main(){
    
    sigset(SIGINT,exit_all);
    
    printf("Unesite broj ciklusa: \n");
    scanf("%d", &nr_ciklusa);
    if(nr_ciklusa<1){
        printf("Broj ciklusa mora biti >= 1!\n");
        exit_all();
    }
    ucitaj_file();
    
    //Inicijalizacija monitorskog semafora
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Mutex init has failed!\n");
        exit_all();
    }
    
    //Inicijalizacija uvjeta
    if(pthread_cond_init(&cond, NULL) != 0){
        printf("Condition init has failed!\n");
        exit_all();
    }
    
    //priprema za ispis
    misle = malloc(sizeof(int)*n_filozofa);
    jedu = malloc(sizeof(int)*n_filozofa);
    cekaju = malloc(sizeof(int)*n_filozofa);
    
    //pokretanje ispisa
    thr_id = malloc(sizeof(pthread_t)*(n_filozofa+1));
    if (pthread_create(thr_id+n_filozofa, NULL, (void*)&ispis, NULL) != 0) {
        printf("Greska pri stvaranju dretve!\n");
        exit_all();
    }
    
    srand(time(NULL));
    //stvaranje filozofa
    param = malloc(sizeof(int)*n_filozofa);
    for(int i=0; i<n_filozofa; i++){
        param[i] = i;
        if (pthread_create(thr_id+i, NULL, (void*)&filozof, param+i) != 0) {
            printf("Greska pri stvaranju dretve!\n");
            exit_all();
        }
        sleep(RAND);
    }
    
    //čekanje da filozofi završe
    for(int i=0; i<n_filozofa; i++)
        pthread_join(thr_id[i], NULL);
    
    exit_all();
    
    return 0;

}
