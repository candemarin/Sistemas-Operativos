#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

int calcular(const char *expresion) {
    int num1, num2, resultado;
    char operador;

    // Usamos sscanf para extraer los dos números y el operador de la expresión
    if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
        printf("Formato incorrecto\n");
        return 0;  // En caso de error, retornamos 0.
    }

    // Realizamos la operación según el operador
    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                resultado = num1 / num2;
            } else {
                printf("Error: División por cero\n");
                return 0;  // Si hay división por cero, retornamos 0.
            }
            break;
        default:
            printf("Operador no reconocido\n");
            return 0;  // Si el operador no es válido, retornamos 0.
    }

    return resultado;
}

void sigint_handler_server(int sig) {
    printf("\nServidor: cerrando socket y saliendo...\n");
    exit(0);
}

void sigint_handler_hijo(int sig) {
    exit(0);
}

int main() {
    int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    unsigned int slen = sizeof(server_addr);
    unsigned int clen = sizeof(client_addr);
    char buffer[1024];

    // Manejo de la señal SIGINT
    // Esto permite cerrar el socket y salir del programa de manera limpia
    // al recibir Ctrl+C.
    signal(SIGINT, sigint_handler_server);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");
    unlink(server_addr.sun_path);

    // Crear el socket
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 1);
    
    printf("Servidor: esperando conexión del cliente...\n");
    while(1) {
        // Aceptar la conexión del cliente
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);
        pid_t pid = fork();
        if (pid == 0) {
            signal(SIGINT, sigint_handler_hijo);
            printf("Servidor: cliente conectado.\n");
            close(server_socket);
            while (1){
                memset(buffer, 0, 1024);
                recv(client_socket, buffer, sizeof(buffer), 0);
                if (strcmp(buffer, "exit") == 0) {
                    printf("Servidor: cliente desconectado.\n");
                    close(client_socket);
                    exit(0);
                }
                
                int resultado = calcular(buffer);
                printf("Servidor: calculando %s = %d\n", buffer, resultado);
                // Enviar el resultado al cliente
                send(client_socket , &resultado, sizeof(resultado), 0);
            }
        } else {
            close(client_socket);
        }
    }
}

