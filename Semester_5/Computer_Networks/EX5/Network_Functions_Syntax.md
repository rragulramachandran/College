# Network Programming Functions Syntax and Arguments

This document explains the syntax of every major socket programming function used in your C programs and details exactly why specific values were passed to them.

---

## 1. `socket()`
**Purpose:** Creates an endpoint for communication and returns a file descriptor.

**Syntax:**
```c
int socket(int domain, int type, int protocol);
```

**Usage in your code:**
```c
sock = socket(AF_INET, SOCK_STREAM, 0);
```

**Why these values?**
- `AF_INET`: This is the **domain** (Address Family). We use `AF_INET` because we are communicating over the IPv4 protocol. (If we were using IPv6, it would be `AF_INET6`).
- `SOCK_STREAM`: This is the socket **type**. We use `SOCK_STREAM` because we want a TCP socket. TCP provides a reliable, two-way, connection-oriented byte stream. (If we wanted UDP, we would use `SOCK_DGRAM`).
- `0`: This is the **protocol**. Setting it to `0` tells the operating system to automatically choose the appropriate protocol for the given domain and type. For `AF_INET` and `SOCK_STREAM`, it will automatically select the TCP protocol (`IPPROTO_TCP`).

---

## 2. `setsockopt()`
**Purpose:** Sets options for the socket, manipulating its behavior.

**Syntax:**
```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

**Usage in your code:**
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

**Why these values?**
- `server_fd`: The socket file descriptor we want to modify.
- `SOL_SOCKET`: The **level** at which the option resides. `SOL_SOCKET` means we are configuring options at the socket API level itself (rather than the TCP or IP level).
- `SO_REUSEADDR`: The **option name**. This specific option allows the server to bind to a port that is currently in the `TIME_WAIT` state (i.e., a port that was recently used and closed). This prevents "Address already in use" errors when you restart your server quickly.
- `&opt`: A pointer to the value we want to set. Since `opt` is `1`, it enables this option (acts as a boolean `true`).
- `sizeof(opt)`: The size of the option value in bytes.

---

## 3. `bind()`
**Purpose:** Associates (binds) a specific IP address and port number to the socket. (Used primarily by servers).

**Syntax:**
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Usage in your code:**
```c
bind(server_fd, (struct sockaddr *)&address, sizeof(address));
```

**Why these values?**
- `server_fd`: The socket file descriptor.
- `(struct sockaddr *)&address`: A pointer to a `sockaddr` structure. We initially filled a `sockaddr_in` structure (which is specific to IPv4) with our desired IP (`INADDR_ANY`) and port. The `bind()` function expects a generic `sockaddr` pointer, so we cast it `(struct sockaddr *)`.
  - *Why `INADDR_ANY`?* It binds the server to **all** available network interfaces on the machine. This means the server will accept connections from any IP address assigned to the computer.
  - *Why `htons(port)`?* The port number must be in network byte order. `htons()` converts it.
- `sizeof(address)`: The exact size of the structure being passed, ensuring the function reads the correct number of bytes.

---

## 4. `listen()`
**Purpose:** Puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.

**Syntax:**
```c
int listen(int sockfd, int backlog);
```

**Usage in your code:**
```c
listen(server_fd, 3);
```

**Why these values?**
- `server_fd`: The socket file descriptor.
- `3`: The **backlog**. This defines the maximum length to which the queue of pending connections for `server_fd` may grow. If a connection request arrives when the queue is full, the client may receive an error. `3` means up to 3 clients can be waiting in line to be `accept()`ed.

---

## 5. `accept()`
**Purpose:** Extracts the first connection request on the queue of pending connections, creates a **new connected socket**, and returns a new file descriptor referring to that socket.

**Syntax:**
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Usage in your code:**
```c
client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
```

**Why these values?**
- `server_fd`: The listening socket.
- `(struct sockaddr *)&address`: A pointer to a structure where the function will write the **client's** IP address and port information once they connect. We use the same `address` struct to store this info.
- `(socklen_t *)&addrlen`: A pointer to an integer containing the size of the structure. It acts as a value-result argument: we pass the allocated size, and it returns the actual size of the client's address written.
- *Return Value (`client_fd`):* This is crucial. The server uses this **new** socket (`client_fd`) to communicate with the client. The original `server_fd` remains open and continues to listen for *other* new connections.

---

## 6. `connect()`
**Purpose:** Initiates a connection on a socket. (Used by clients).

**Syntax:**
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Usage in your code:**
```c
connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
```

**Why these values?**
- `sock`: The client's socket file descriptor.
- `(struct sockaddr *)&serv_addr`: A pointer to a structure containing the **server's** IP address and port that the client wants to connect to.
  - *Why `inet_addr("127.0.0.1")`?* `127.0.0.1` is the loopback address (localhost). It tells the client to connect to a server running on the **same computer**. `inet_addr` converts this string into the binary format needed by the struct.
- `sizeof(serv_addr)`: The size of the address structure.

---

## 7. `send()` / `write()`
**Purpose:** Sends data to a connected socket.

**Syntax:**
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// OR
ssize_t write(int fd, const void *buf, size_t count);
```

**Usage in your code:**
```c
send(sock, message, strlen(message), 0);
// OR
write(sock, &n, sizeof(n));
```

**Why these values?**
- `sock` / `client_fd`: The socket file descriptor connected to the peer.
- `message` / `&n`: A pointer to the buffer containing the data to be sent. For strings, it's the character array. For variables like an integer `n`, we must pass its memory address using `&n`.
- `strlen(message)` / `sizeof(n)`: The exact number of bytes to send. For strings, we use `strlen()` (we don't send the null terminator). For data types, we use `sizeof()` to ensure all bytes of the integer/array are sent.
- `0` (for `send`): The **flags** argument. `0` means standard behavior, making `send()` functionally identical to `write()`.

---

## 8. `recv()` / `read()`
**Purpose:** Receives data from a connected socket.

**Syntax:**
```c
ssize_t read(int fd, void *buf, size_t count);
```

**Usage in your code:**
```c
int valread = read(client_fd, buffer, 1024);
// OR
read(sock, &sum, sizeof(sum));
```

**Why these values?**
- `client_fd` / `sock`: The connected socket file descriptor to read from.
- `buffer` / `&sum`: A pointer to the memory location where the received data will be stored. For strings, it's a char array. For a single integer `sum`, it's `&sum`.
- `1024` / `sizeof(sum)`: The **maximum** number of bytes to read. For buffers, you provide the buffer's size to prevent buffer overflows. For variables, you provide `sizeof(variable)` to read exactly that many bytes.
- *Return Value (`valread`):* Returns the actual number of bytes read. If it's a string, we manually add the null terminator `\0` at index `valread` to make it a valid C string.

---

## 9. `close()`
**Purpose:** Closes a file descriptor, so that it no longer refers to any file and may be reused.

**Syntax:**
```c
int close(int fd);
```

**Usage in your code:**
```c
close(sock);
```

**Why these values?**
- `sock`: It simply frees the operating system resources allocated to that socket and gracefully terminates the TCP connection.
