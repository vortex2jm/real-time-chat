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

int main(void)
{
  int client_socket;
  SockAddrIn server_addr;

  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Error creating client socket!\n");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connecting socket to server
  if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("Error connecting to server!\n");
    exit(1);
  }

  printf("Connected to server!\n");
  //=====================================

  pthread_t threads[2];

  if (pthread_create(&threads[0], NULL, send_routine, &client_socket))
  {
    perror("Error creating send thread!\n");
    exit(1);
  }
  if (pthread_create(&threads[1], NULL, receive_routine, &client_socket))
  {
    perror("Error creating receive thread!\n");
    exit(1);
  }
  for (int x = 0; x < 2; x++)
  {
    pthread_join(threads[x], NULL);
  }

  close(client_socket);

  return 0;
}

// Thread 0 routine
void *send_routine(void *args){
  int client_socket = *(int*)args; 
  char buffer[1024];
  memset(buffer, 0x0, sizeof(buffer));
  printf("Chat is ready!\n");
  while(1){
    fgets(buffer, sizeof(buffer), stdin);
    if(!strcmp(buffer, "exit\n")){
      printf("Closing connection...\n");
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

//Thread 1 routine
void *receive_routine(void *args){
  int client_socket = *(int*)args; 
  char buffer[1024];
  memset(buffer, 0x0, sizeof(buffer));
  ssize_t received_bytes=0;
  while(1){
    if(!(received_bytes = recv(client_socket, buffer, sizeof(buffer), 0))){
      printf("Connection closed by server. Type \"exit\" to close!\n");
      break;
    }
    if(received_bytes < 0){
      printf("Error receiving data from client. Type \"exit\" to close and try a reconnection!\n");
      break;
    }
    buffer[received_bytes] = 0x0;
    printf("server -> %s\n", buffer);
    memset(buffer, 0x0, sizeof(buffer));
  }  
  pthread_exit(NULL);
}
