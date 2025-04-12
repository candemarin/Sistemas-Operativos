#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
    return (rand() % 50);
}

void starterProcess(int toRead, int toWrite, int fromParent, int toParent) {
    int randomNumber = generate_random_number();
    int counter;

    printf("Este es el random number %d\n", randomNumber);
    
    read(fromParent, &counter, sizeof(counter));
    write(toWrite, &counter, sizeof(counter));

    while (1) {
        read(toRead, &counter, sizeof(counter));
        if (counter >= randomNumber) {
            write(toParent, &counter, sizeof(counter));
            exit(0);
        } else {
        counter++;
        printf("Este es el counter %d\n", counter);
        write(toWrite, &counter, sizeof(counter));
        }
    }
}

void otherProcess(int toRead, int toWrite) {
    int counter;
    while (1) {
        if (read(toRead, &counter, sizeof(counter)) == -1) {
            exit(0);
        } else {
            counter++;
            printf("Este es el counter %d\n", counter);
            write(toWrite, &counter, sizeof(counter));
        }
    }
}



int main(int argc, char **argv)
{   
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL));
    
    int status, pid, n, start, buffer;
    n = atoi(argv[1]);
    buffer = atoi(argv[2]);
    start = atoi(argv[3]);

    if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);

    int pipes[n][2];
    for (size_t i = 0; i < n; i++){
        pipe(pipes[i]);
    }

    int fatherToStarter[2];
    pipe(fatherToStarter);

    // Must check
    //write(fatherToStarter[PIPE_WRITE], &buffer, sizeof(buffer));

    for (size_t i = 0; i < n; i++){
        if (fork() == 0){
            if (i == start) {
                starterProcess(pipes[i % n][PIPE_READ], pipes[(i + 1) % n][PIPE_WRITE], fatherToStarter[PIPE_READ], fatherToStarter[PIPE_WRITE]);
            } else {
                otherProcess(pipes[i % n][PIPE_READ], pipes[(i + 1) % n][PIPE_WRITE]);
            }
        }
    }

    // El padre cierra todos los pipes
    for (int i = 0; i < n; i++) {
        close(pipes[i][PIPE_WRITE]);
        close(pipes[i][PIPE_READ]);
    }

    // must check

    
    write(fatherToStarter[PIPE_WRITE], &buffer, sizeof(buffer));

    int result;

    sleep(1);
    //deberiamos de cerrar los pipes

    read(fatherToStarter[PIPE_READ], &result, sizeof(result));

    printf("El valor acumulado es %d\n", result);
    
    exit(0);
}
