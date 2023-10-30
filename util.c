#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int master_fd = -1;
pthread_mutex_t accept_con_mutex = PTHREAD_MUTEX_INITIALIZER;





/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - if init encounters any errors, it will call exit().
************************************************/
void init(int port) {
   int sd;
   struct sockaddr_in addr;
   //fill in addr struct
   addr.sin_family= AF_INET; 
   addr.sin_addr.s_addr= INADDR_ANY;  //use macro to convert add. Int to network format. INADDR_ANY - get me any local IP address. If multi-homed may want to choose yourself. 
   addr.sin_port= htons(port); // server picks the port

   int ret_val;
   int flag = 1;
  
   
   
   /**********************************************
    * IMPORTANT!
    * ALL TODOS FOR THIS FUNCTION MUST BE COMPLETED FOR THE INTERIM SUBMISSION!!!!
    **********************************************/
   
   
   
   // Create a socket and save the file descriptor to sd (declared above), exit if failed
   if ((sd = socket(AF_INET, SOCK_STREAM,  0))<0){
      perror("Error: socket() failed");
      exit(1);
   }
   // This socket should be for use with IPv4 and for a TCP connection. 

   // Change the socket options to be reusable using setsockopt(), exit if failed
    if ((ret_val = (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)))) < 0){
      perror("Error: setsockopt() failed");
      exit(1);

    }

   // Bind the socket to the provided port, exit if failed
   if (bind(sd,(struct sockaddr *) &addr, sizeof(addr)) < 0){
      perror("Error: bind() failed");      
      exit(1);
   }

   // Mark the socket as a pasive socket. (ie: a socket that will be used to receive connections), exit if failed
   if (listen(sd, 20) < 0){
      perror("Error: listen() failed");
      exit(1);
   }
      
   // We save the file descriptor to a global variable so that we can use it in accept_connection()
   master_fd = sd;
   printf("UTILS.O: Server Started on Port %d\n",port);

}





/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
     DO NOT use the file descriptor on your own -- use
     get_request() instead.
   - if the return value is negative, the thread calling
     accept_connection must should ignore request.
***********************************************/
int accept_connection(void) {
   int newsock;
   struct sockaddr_in new_recv_addr;
   uint addr_len;
   addr_len = sizeof(new_recv_addr);
   
   /**********************************************
    * IMPORTANT!
    * ALL TODOS FOR THIS FUNCTION MUST BE COMPLETED FOR THE INTERIM SUBMISSION!!!!
    **********************************************/
   
   
   
   // Aquire the mutex lock, print error if failed 
   if (pthread_mutex_lock(&accept_con_mutex) < 0){
    printf("Error: Failed to lock mutex connection.\n");
   }

   // Accept a new connection on the passive socket and save the fd to newsock, exit if failed
  if ((newsock = accept(master_fd, (struct sockaddr *) &new_recv_addr, &addr_len))<0){
    perror("Error: accept() failed.\n");
    exit(1);
  }

   // Release the mutex lock, print error if failed
   if (pthread_mutex_unlock(&accept_con_mutex) < 0) {
    printf("Error: Failed to unlock mutex connection.\n");
   }

   // Return the file descriptor for the new client connection
   return newsock;
}





/**********************************************
 * get_request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        from where you wish to get a request
      - filename is the location of a character buffer in which
        this function should store the requested filename. (Buffer
        should be of size 1024 bytes.)
   - returns 0 on success, nonzero on failure. You must account
     for failures because some connections might send faulty
     requests. This is a recoverable error - you must not exit
     inside the thread that called get_request. After an error, you
     must NOT use a return_request or return_error function for that
     specific 'connection'.
************************************************/
int get_request(int fd, char *filename) {

      /**********************************************
    * IMPORTANT!
    * THIS FUNCTION DOES NOT NEED TO BE COMPLETE FOR THE INTERIM SUBMISSION, BUT YOU WILL NEED
    * CODE IN IT FOR THE INTERIM SUBMISSION!!!!! 
    **********************************************/
    
    
   char buf[2048];
   
   // Read the request from the file descriptor into the buffer, print error if failed 
   if (read(fd, buf, 2048) < 1){
    fprintf(stderr,"Error: Failed to read request.\n");
    return -1;
   }
   
   //read first line, print error if failed
   char *line = strtok_r(buf, "\n", &line); 
   if (line == NULL) {
    fprintf(stderr,"Error: Failed to get first line of request.\n");
    return -1;
   }

      
   // Print the first line of the request to the terminal.
   //fprintf(stdout, "%s\n", line);
   
   char *first_word = strtok_r(line, " ", &line);
   char *second_word = strtok_r(NULL," ", &line);
   char *third_word = strtok_r(NULL," ",&line);

    // print error if no words were read
   if (first_word == NULL || second_word == NULL || third_word == NULL) {
    fprintf(stderr,"Error: Failed to get words from first line of request.\n");
    return -1;
   }

   // Ensure that the incoming request is a properly formatted HTTP "GET" request
   // The first line of the request must be of the form: GET <file name> HTTP/1.0 
   // or: GET <file name> HTTP/1.1

   if (strcmp(first_word, "GET") != 0) {
    fprintf(stderr, "Error: request was not an HTTP GET request.\n");\
    return -1;
   }

   //  Extract the file name from the request

   if (strlen(third_word) > 7) {
    third_word[8] = '\0';
   }

   if (strcmp(third_word, "HTTP/1.0") != 0 && strcmp(third_word,"HTTP/1.1") != 0) {
    fprintf(stderr,"Error: request was not an HTTP GET request.\n");
    fprintf(stderr,"third word is: %s\n",third_word);
    return -1;
   }
   
      
   // Ensure the file name does not contain with ".." or "//"
   // FILE NAMES WHICH CONTAIN ".." OR "//" ARE A SECURITY THREAT AND MUST NOT BE ACCEPTED!!!
   // We print out errors and return -1 for these cases 
   if (strstr(second_word,"..") != NULL) {
    fprintf(stderr, "Error: unsafe request.\n");
    return -1;
   }

   if (strstr(second_word,"//") != NULL) {
    fprintf(stderr, "Error: unsafe request.\n");
    return -1;
   }
   

   // Copy the file name to the provided buffer
   strncpy(filename, second_word, 1024);

   return 0;
}





/**********************************************
 * return_result
   - returns the contents of a file to the requesting client
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the result of a request
      - content_type is a pointer to a string that indicates the
        type of content being returned. possible types include
        "text/html", "text/plain", "image/gif", "image/jpeg" cor-
        responding to .html, .txt, .gif, .jpg files.
      - buf is a pointer to a memory location where the requested
        file has been read into memory (the heap). return_result
        will use this memory location to return the result to the
        user. (remember to use -D_REENTRANT for CFLAGS.) you may
        safely deallocate the memory after the call to
        return_result (if it will not be cached).
      - numbytes is the number of bytes the file takes up in buf
   - returns 0 on success, nonzero on failure.
************************************************/
int return_result(int fd, char *content_type, char *buf, int numbytes) {

   // Prepare the headers for the response you will send to the client.
    FILE *ptr =fdopen(fd, "w");

   /* EXAMPLE HTTP RESPONSE
    * 
    * HTTP/1.0 200 OK
    * Content-Length: <content length>
    * Content-Type: <content type>
    * Connection: Close
    * 
    * <File contents>
    */
    
    // TODO: Send the HTTP headers to the client
    
    if (ptr == NULL){
      printf("Error: failed to open file.\n");
      close (fd);
      return -4;
    }
    
    // print headers and the file contents, as in the example above.
    if (fprintf(ptr, "HTTP/1.0 200 OK\nContent-Length: %d\nContent-Type: %s\nConnection: Close\n\n", numbytes, content_type)<0){
      printf("Error: failed to print result.\n");
      close (fd);
      return -3;
    }

    if (fflush(ptr)<0){
      printf("Error: failed to flush.\n");
      close (fd);
      return -2;
    }
    
    // Send the file contents to the client
    if (write(fd, buf, numbytes) < numbytes){
      printf("Error: failed to write data back to client. \n");
      close (fd);
      return -1;
    }

    // Close the connection to the client
    close (fd);

    return 0;
}





/**********************************************
 * return_error
   - returns an error message in response to a bad request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the error
      - buf is a pointer to the location of the error text
   - returns 0 on success, nonzero on failure.
************************************************/
int return_error(int fd, char *buf) {

   // Prepare the headers to send to the client
    FILE * ptr =fdopen(fd, "w");

   
   /* EXAMPLE HTTP ERROR RESPONSE
    * 
    * HTTP/1.0 404 Not Found
    * Content-Length: <content length>
    * Connection: Close
    * 
    * <Error Message>
    */

    // Send headers to the client
    if (ptr == NULL){
      printf("Error: failed to open file.\n");
      close (fd);
      return -4;
    }

    // print headers and the file contents, as in the example above.
    if (fprintf(ptr, "HTTP/1.0 404 Not Found\nContent-Length: %ld\nConnection: Close\n\n", strlen(buf))<0){
      printf("Error: failed to print error.\n");
      close(fd);
      return -3;
    }

    if (fflush(ptr)<0){
      printf("Error: failed to flush.\n");
      close(fd);
      return -2;
    }

    // Send the error message to the client
    if (write(fd, buf, strlen(buf)) < strlen(buf)){
      printf("Error: failed to write error back to client. \n");
      close (fd);
      return -1;
    }

    // Close the connection with the client.
    close(fd);
    return 0;
}
