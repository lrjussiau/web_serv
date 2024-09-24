
# Webserv - HTTP Server Implementation

## Description

*Webserv* is a project from 42 school where you are tasked with building a simple HTTP server in C++. The goal is to develop a web server that adheres to HTTP/1.1 standards, supporting features such as handling multiple clients, serving static files, managing GET, POST, and DELETE requests, and more. This project focuses on network programming, socket management, and understanding the HTTP protocol.

## Project Objectives

- Implement an HTTP/1.1-compliant web server in C++.
- Manage multiple client connections using non-blocking I/O.
- Handle HTTP methods (GET, POST, DELETE).
- Serve static files and manage dynamic routes.
- Implement error handling and generate HTTP responses.

## Features

1. **HTTP Request Handling**
   - Parse and respond to HTTP/1.1 requests.
   - Support for `GET`, `POST`, and `DELETE` methods.

2. **Client-Server Communication**
   - Handle multiple client connections simultaneously.
   - Use non-blocking I/O (e.g., `select()`, `poll()`, or `epoll()`).
   - Manage HTTP headers and response codes.

3. **File Handling**
   - Serve static files from a specified directory (e.g., HTML, CSS, JS files).
   - Handle file uploads through POST requests.

4. **Configuration**
   - Use a configuration file to set server properties such as listening ports, server names, and root directories.

5. **Error Pages**
   - Serve custom error pages for 404, 403, 500, etc.
   - Gracefully handle malformed requests.

6. **CGI Support (Optional)**
   - Support Common Gateway Interface (CGI) for dynamic content generation.

## System Calls and Functions

Key functions and system calls used in the project include:

- `socket()`: Create a socket.
- `bind()`: Bind a socket to an IP address and port.
- `listen()`: Start listening for incoming connections.
- `accept()`: Accept incoming client connections.
- `recv()` and `send()`: Receive and send data through sockets.
- `select()`, `poll()`, `epoll()`: Handle multiple clients using non-blocking I/O.
- `fork()`, `execve()`: Optionally used for CGI handling.

## Installation

To compile and run the *Webserv* project, follow these steps:

```bash
git clone https://github.com/your-repo/webserv.git
cd webserv
make
```

Run the server with a configuration file:

```bash
./webserv config/webserv.conf
```

## Configuration

The server is configured using a `.conf` file that defines server properties such as:

- **Listening Port**: The port on which the server will listen for incoming requests.
- **Server Name**: The domain or IP address.
- **Root Directory**: The directory from which static files will be served.
- **Error Pages**: Paths to custom error pages (404, 500, etc.).

Example configuration:

```
server {
    listen 8080;
    server_name localhost;
    root /var/www/html;
    error_page 404 /errors/404.html;
}
```

## Usage

After running the server, you can open a web browser and connect to it:

```bash
http://localhost:8080
```

The server will serve static files from the root directory specified in the configuration file. You can also send HTTP requests using tools like \`curl\`:

```bash
curl -X GET http://localhost:8080/index.html
```

## Error Handling

The server should handle:

- Invalid HTTP requests and malformed headers.
- Unsupported HTTP methods.
- File not found (404) or access denied (403).
- Connection timeouts or client disconnections.

## Testing

Test the server by serving static files and sending various HTTP requests (GET, POST, DELETE). Use tools like \`curl\` or Postman to test different scenarios.

Example of a file upload via POST:

```bash
curl -F "file=@test.txt" http://localhost:8080/upload
```

Verify that the file is uploaded correctly and the server responds with appropriate status codes.
