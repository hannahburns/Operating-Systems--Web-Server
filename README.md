
# CSCI 4061: Web Server Socket Programming

## Overview
This repository contains the code for Project 4 in CSCI 4061: Introduction to Operating Systems, instructed by Jon Weissman.

In Project 3, we developed a multi-threaded web server using POSIX threads and synchronization mechanisms. In this project, we focus on implementing the networking utility functions using POSIX socket programming. These utility functions are responsible for receiving requests and sending responses between clients and the server.

## Description
The core of this project is the implementation of five utility functions in `util.c`. These functions are used for initializing the connection system, accepting connections, handling requests, and returning responses. The interface of each of these functions is described in detail in the provided util.h header file with additional comments in utils.c file. A high-level overview of what each function should do is provided below.

### Initialization (init)
The `init(int port)` function initializes the connection acceptance and handling system. It sets up a socket for receiving client connection requests on the provided port. If an error occurs during initialization, the function will call `exit()`.

### Accepting Connections (accept_connection)
The `accept_connection(void)` function is called to accept a new client connection request. It returns a file descriptor for further request processing. If the return value is negative, the calling thread should ignore the request.

### Handling Requests (get_request)
The `get_request(int fd, char *filename)` function retrieves a request from the client and stores the requested filename in the provided buffer. It returns 0 on success and a non-zero value on failure. This function ensures that the incoming request is a properly formatted HTTP "GET" request and that the file name does not contain unsafe characters.

### Returning Results (return_result)
The `return_result(int fd, char *content_type, char *buf, int numbytes)` function returns the contents of a file to the requesting client. It takes parameters such as the file descriptor, content type, and file buffer. The function prepares and sends the HTTP headers to the client, followed by the file contents. It returns 0 on success and a non-zero value on failure.

### Returning Errors (return_error)
The `return_error(int fd, char *buf)` function returns an error message in response to a bad request. It takes the file descriptor and the error message buffer as parameters. The function prepares and sends the HTTP error headers to the client, followed by the error message. It returns 0 on success and a non-zero value on failure.

## How to Run the Server
To run the server, execute the following command:

```shell
./web_server <port> <path>
```

The server no longer requires command-line arguments for threads, cache, or request queue, as these are handled in `server.o`.

## Provided Files
This repository includes the following files:

- `util.c`: The main file that contains the implementation of the networking utility functions.
- `server.o`: A solution for the Project 3 portion of the web server.
- `util.h`, `Makefile`, `testing.zip`, `web_server_sol`: Provided files from Project 3.

## How to Test Your Server
Refer to section 11 of Project 3 for detailed testing instructions. To test `return_result()` and `return_error()`, you can use `wget` with the "-S" and "--content-on-error" flags to examine the HTTP headers and retrieve error messages.

For example:

```shell
% wget -S http://127.0.0.1:9000/image/gif/0.gif
```

```shell
% wget --content-on-error http://127.0.0.1:9000/image/gif/not_a_file
% cat not_a_file
```

## Defines and Simplifying Assumptions
- The server should accept a backlog of up to 20 connection requests.
- The maximum size of a GET request is 2047 bytes.
- The maximum size of a GET response header is 2047 bytes (for the header, the file size or error message may be larger).
  
## License

Copyright ©  
 * Group Member #1: Yanai Sun sun00105
 * Group Member #2: Leita Nguyen nguy3599
 * Group Member #3: Hannah Burns burns0460
2022

This work is licensed under a [Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License](https://creativecommons.org/licenses/by-nc-nd/4.0/).

This means that you are free to:

- Share — You can copy and redistribute the material in any medium or format.

Under the following terms:

- Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

- NonCommercial — You may not use the material for commercial purposes.

- NoDerivatives — If you remix, transform, or build upon the material, you may not distribute the modified material.

You are free to use this work for personal and educational purposes, but you may not use it for commercial gain, and you cannot create derivative works based on it without permission.
