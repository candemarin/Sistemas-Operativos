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

	pid_t starterPid;

    for (size_t i = 0; i < n; i++){
        if (fork() == 0){

			for (int j = 0; j < n; j++) {
				if (j == i % n) {
					//close(pipes[j][PIPE_WRITE]);
				} else if (j == (i + 1) % n) {
					//close(pipes[j][PIPE_READ]);
				} else {
					close(pipes[j][PIPE_WRITE]);
					close(pipes[j][PIPE_READ]);
				}
			}

            if (i == start) {
				starterPid = getpid();
                starterProcess(pipes[i % n][PIPE_READ], pipes[(i + 1) % n][PIPE_WRITE], fatherToStarter[PIPE_READ], fatherToStarter[PIPE_WRITE]);
            } else {
				close(fatherToStarter[PIPE_WRITE]);
				close(fatherToStarter[PIPE_READ]);
                otherProcess(pipes[i % n][PIPE_READ], pipes[(i + 1) % n][PIPE_WRITE]);
            }
        }
    }

    // El padre cierra todos los pipes
    for (int i = 0; i < n; i++) {
        close(pipes[i][PIPE_WRITE]);
        close(pipes[i][PIPE_READ]);
    }

    write(fatherToStarter[PIPE_WRITE], &buffer, sizeof(buffer));

    int result;

	// sleep(1);
	// deberiamos cerrar los pipes

	waitpid(starterPid, &status, 0);

    read(fatherToStarter[PIPE_READ], &result, sizeof(result));

    printf("El valor acumulado es %d\n", result);
    
	close(fatherToStarter[PIPE_WRITE]);
	close(fatherToStarter[PIPE_READ]);

    exit(0);
}
