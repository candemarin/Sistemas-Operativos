#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int n;
int numero_maldito;
int *estados; 
pid_t *pids; 


int generate_random_number(){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL) ^ getpid());
    return (rand() % n);
}

void signal_handler(int sig){
    if(sig == SIGTERM){
        int numero = generate_random_number();
        printf("hijo %d, genere el numero: %d\n", getpid(), numero);
        if(numero == numero_maldito){
            printf("estas son mis ultimas palabras, pid: %d\n", getpid());
            kill(getpid(), SIGKILL); // exit(0);
        }
    }
}

void signal_child_handler(int sig){
    if(sig == SIGCHLD){
        pid_t pid_muerto = wait(NULL);
        for(int i = 0; i < n; i++){
            if(pids[i] == pid_muerto){
                estados[i] = 0;
                break;
            }
        }
    }
}

int main(int argc, char const *argv[]){
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
    pids = malloc(n * sizeof(pid_t));
    estados = malloc(n * sizeof(int));
	numero_maldito = atoi(argv[3]);
    for(int i = 0; i < n; i++){
        estados[i] = 1;
    }

    for(int j = 0; j < n; j++){
        int pid = fork();
        if (pid == 0) {
            signal(SIGTERM, signal_handler);
            while(1);
        }
        pids[j] = pid;
    }
    signal(SIGCHLD, signal_child_handler);
    for(int k = 0; k < rondas; k++){ 
        printf("Ronda %d\n", k);
        for(int i = 0; i < n; i++){
            // padre
            if (estados[i] == 1){
                kill(pids[i], SIGTERM);
                sleep(1);
            }
        }
    }

    printf("Ganadores:\n");
    for(int i = 0; i < n; i++){
        if(estados[i] == 1){
            printf("%d -> %d \n", i, pids[i]);
            kill(pids[i], SIGKILL);
        }
    }

    exit(0);
}