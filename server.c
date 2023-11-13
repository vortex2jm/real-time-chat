#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct sockaddr_in SockAddrIn;
void *send_routine(void *args);
void *receive_routine(void *args);
int server_socket;

int main(void)
{
  int client_socket;
  SockAddrIn server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  // Creating server socket
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Error creating server socket!\n");
    exit(1);
  }

  // Setting up server
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
  {
    perror("Error assigning a port to server socket\n");
    exit(1);
  }

  // Socket's listening...
  if (listen(server_socket, 5) == -1) // The second parameters is the length of connection queue
  {
    perror("Error listening connections!\n");
    close(server_socket);
    exit(1);
  }

  printf("Server's listening on port 8080!\n");

  // Connecting to client (instantiating a new socket with client)
  if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
  {
    perror("Error accepting client connection!\n");
    close(server_socket);
    exit(1);
  }

  printf("Client connected!\n");
  //===================================================

  pthread_t threads[2];
  
  if(pthread_create(&threads[0], NULL, send_routine, &client_socket)){  // Passing client connection as args
    perror("Error creating send thread!\n");
    exit(1);
  }
  if(pthread_create(&threads[1], NULL, receive_routine, &client_socket)){
    perror("Error creating receive thread!\n");
    exit(1);
  }
  for(int x=0; x<2;x++){
    pthread_join(threads[x], NULL);
  }

  close(server_socket);
  close(client_socket);

  return 0;
}

//========================//
void *send_routine(void *args)
{
  int client_socket = *(int*)args; 
  char buffer[1024];
  memset(buffer, 0x0, sizeof(buffer));
  printf("Chat is ready!\n");
  while(1){
    fgets(buffer, sizeof(buffer), stdin);
    if(!strcmp(buffer, "exit\n")){
      printf("Closing connection...\n");
      close(server_socket);
      close(client_socket);
      sleep(2);
      exit(0);
    }
    if(send(client_socket, buffer, strlen(buffer), 0) == -1){
      perror("Error. Message has not been sent!\n");
    }
    memset(buffer, 0x0, sizeof(buffer));
  }
  pthread_exit(NULL);
}

//=======================================//
void *receive_routine(void *args)
{
  int client_socket = *(int*)args; 
  char buffer[1024];
  memset(buffer, 0x0, sizeof(buffer));
  ssize_t received_bytes=0;
  while(1){
    if(!(received_bytes = recv(client_socket, buffer, sizeof(buffer), 0))){
      printf("Connection closed by client. Type \"exit\" to close!\n");
      break;
    }

    if(received_bytes < 0){
      printf("Error receiving data from client. Type \"exit\" to close and try a reconnection!\n");
      break;
    }
    buffer[received_bytes] = 0x0;
    printf("client -> %s\n", buffer);
    memset(buffer, 0x0, sizeof(buffer));
  }  
  pthread_exit(NULL);
}
