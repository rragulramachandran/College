# Network Programming Question and Answer Set

This document contains a set of common viva/interview questions based on your 6 C network programs: TCP Echo Client/Server, Array Sum Client/Server, and Bit Stuffing Client/Server. These will help you prepare for any questions your teacher might ask.

## General Socket Programming Questions

**Q1: What are the basic steps to create a TCP server?**
**A:** The basic steps are:
1. **`socket()`**: Create a socket.
2. **`setsockopt()`** (optional): Set socket options like `SO_REUSEADDR` to quickly reuse the port.
3. **`bind()`**: Bind the socket to a specific IP address and port.
4. **`listen()`**: Listen for incoming connections (puts the socket in a passive state).
5. **`accept()`**: Accept an incoming client connection.
6. **`read()` / `write()`** (or `recv` / `send`): Communicate with the connected client.
7. **`close()`**: Close the connection and the socket.

**Q2: What are the basic steps to create a TCP client?**
**A:** The basic steps are:
1. **`socket()`**: Create a socket.
2. **`connect()`**: Connect the socket to the server's IP address and port.
3. **`write()` / `read()`** (or `send` / `recv`): Send data to and receive data from the server.
4. **`close()`**: Close the socket.

**Q3: What does the `socket(AF_INET, SOCK_STREAM, 0)` function call do?**
**A:** It creates a new socket for network communication. 
- `AF_INET` specifies the IPv4 address family.
- `SOCK_STREAM` specifies that this is a reliable, connection-oriented TCP socket.
- `0` tells the system to choose the default protocol for this family and type (which is IPPROTO_TCP).

**Q4: Explain the `sockaddr_in` structure.**
**A:** `sockaddr_in` is a C structure used to store an IPv4 address and port number. Its main fields are:
- `sin_family`: Address family (must be set to `AF_INET`).
- `sin_port`: Port number, converted to network byte order using `htons()`.
- `sin_addr.s_addr`: IP address. `INADDR_ANY` is used for servers to bind to all available local interfaces, while clients use `inet_addr("127.0.0.1")` to connect to localhost.

**Q5: What is the purpose of `htons()` and why is it necessary?**
**A:** `htons()` stands for "host to network short". It converts a 16-bit integer (like a port number) from the host machine's byte order (often little-endian) to the network byte order (which is strictly big-endian). This ensures the port is correctly interpreted by the network stack regardless of the machine's architecture.

## 1. TCP Echo Program (`Client.c` & `Server.c`)

**Q6: What port does the Echo server listen on?**
**A:** The Echo server listens on port **6000**.

**Q7: How does the server handle client connections in `Server.c`?**
**A:** In your `Server.c`, it only accepts a single client connection, reads one message, echoes it back, and then terminates. It does not use a `while(1)` loop around the `accept()` call to handle multiple clients sequentially. 

**Q8: What function is used to send the message in `Client.c`?**
**A:** `send(sock, message, strlen(message), 0);` is used to send the message string to the server.

## 2. Array Sum Program (`Array_Client.c` & `Array_Server.c`)

**Q9: How is an array of integers transmitted from the client to the server over TCP?**
**A:** TCP provides a continuous stream of bytes, not structured arrays. To send an array, the client first sends the size of the array (`n`), and then sends the actual array memory block using `write(sock, arr, n * sizeof(int));`.

**Q10: Why does the Array Server use a `while(1)` loop?**
**A:** The server uses `while(1)` to run continuously. After accepting a client, receiving the array, and returning the sum, it loops back to `accept()` to wait for the next client connection without terminating.

**Q11: How does the client receive the integer sum back from the server?**
**A:** The client uses `read(sock, &sum, sizeof(sum));` to read the raw bytes of the returned integer directly into the memory address of the `sum` variable.

## 3. Bit Stuffing Program (`Bit_Client.c` & `Bit_Server.c`)

**Q12: What is Bit Stuffing and why is it used?**
**A:** Bit stuffing is the process of inserting a non-information bit into data to break up bit patterns that could be accidentally mistaken for control characters (like frame flags, often `01111110`). In this program, a `0` is automatically inserted after every five consecutive `1`s.

**Q13: How does `Bit_Client.c` convert a text message to binary?**
**A:** It takes every character of the text, extracts its ASCII value, and uses bitwise shifts (`(ch >> k) & 1`) inside a loop (from k=7 down to 0) to extract each of the 8 bits and append them as '0' or '1' characters to a binary string.

**Q14: Explain the logic for bit stuffing used in the code.**
**A:** The program iterates through the binary string character by character. 
- If it encounters a '1', it increments a counter. 
- If the counter reaches 5, it appends the '1', immediately appends a '0' to the stuffed string, and resets the counter to 0. 
- If it encounters a '0', it simply appends it and resets the counter.

**Q15: How does the server perform bit destuffing?**
**A:** The server reads the stuffed binary string and counts consecutive '1's. When the count hits five, it knows the very next character must be a stuffed '0', so it increments the reading index `i` to skip over that '0', ignoring it, and resets the counter.

**Q16: How is the binary data converted back to text in the server?**
**A:** The `binaryToText` function takes blocks of 8 characters ('0' and '1'). For each block, it initializes an empty character `ch = 0`, and uses bitwise left shifts and OR operations (`ch = (ch << 1) | (byte[k] - '0')`) to reconstruct the original 8-bit ASCII character.
