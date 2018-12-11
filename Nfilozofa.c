#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int n_filozofa = 0;
int n_stapica = 0;
int *n_ruku;

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

int main(){
         
    ucitaj_file();
    
    /*
    printf("%d %d\n", n_stapica, n_filozofa);
    for(int i=0; i<n_filozofa; i++)
        printf("%d\n",n_ruku[i]);
    */
    
    return 0;

}
