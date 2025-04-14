#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

void trim(char *str) {
    char *start = str; // Pointer to the start of the string
    char *end;

    // Trim leading whitespace
    while (isspace((unsigned char)*start)) {
        start++;
    }

    // If the string is all spaces, set it to an empty string
    if (*start == 0) {
        str[0] = '\0';
        return;
    }

    // Trim trailing whitespace
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    // Write the null terminator after the last non-whitespace character
    *(end + 1) = '\0';

    // Move the trimmed string back to the original buffer
    memmove(str, start, end - start + 2);
}

int server_socket;

void sigint_handler(int sig) {
    write(server_socket, "exit", sizeof("exit"));
	printf("\nCliente: cerrando conexión...\n");
	close(server_socket);
	exit(0);
}

int main() {
	signal(SIGPIPE, SIG_IGN);

	// Manejo de la señal SIGINT
    // Esto permite cerrar el socket y salir del programa de manera limpia
    // al recibir Ctrl+C.
    signal(SIGINT, sigint_handler);

    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error");
        exit(1);
    }
	
	
	char cuenta[1024]; // Allocate a buffer to store the input
	while(1) {
		printf("Enter a expression (or exit): ");
		fgets(cuenta, sizeof(cuenta), stdin);
		trim(cuenta);
		if (strcmp(cuenta,"exit") == 0) {
			sigint_handler(2);			
		}

		int resultado;

		printf("Cliente: enviando %s al servidor...\n", cuenta);
		
    	if (write(server_socket, &cuenta, sizeof(cuenta)) == -1) {
			printf("El servidor ha sido cerrado\n");
			close(server_socket);
			exit(0);
		};
	
		printf("Cliente: esperando respuesta del servidor...\n");
    
		if (read(server_socket, &resultado, sizeof(resultado)) == 0) {
			printf("El servidor ha sido cerrado\n");
			close(server_socket);
			exit(0);
		};
    
		printf("Cliente: recibí %d del servidor!\n", resultado);

	}
}
