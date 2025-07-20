# TCP echo server with concurrent clients using `fork()`

## Run

### server

```bash
❯ ./server
Server listening on port 8080...
Parent continuing with PID: 79219
Forced child process for the client: [PID:79238|127.0.0.1:55127]
Received from client:[PID:79238|127.0.0.1:55127] hello from the first client
Parent continuing with PID: 79219
Forced child process for the client: [PID:79307|127.0.0.1:55128]
Received from client:[PID:79307|127.0.0.1:55128] hello from the second client
Received from client:[PID:79238|127.0.0.1:55127] bye from the first client
Client has closed the connection
Finished sesion with the client: [PID:79238|127.0.0.1:55127]
Received from client:[PID:79307|127.0.0.1:55128] bye from the second client
Client has closed the connection
Finished sesion with the client: [PID:79307|127.0.0.1:55128]
^C
```

### client-1

```bash
❯ ./client
Enter the message to send to server (type 'exit' to quit):
hello from the first client
Server replied: hello from the first client
Enter the message to send to server (type 'exit' to quit):
bye from the first client
Server replied: bye from the first client
Enter the message to send to server (type 'exit' to quit):
exit
Client exiting...
```

### client-2

```bash
❯ ./client
Enter the message to send to server (type 'exit' to quit):
hello from the second client
Server replied: hello from the second client
Enter the message to send to server (type 'exit' to quit):
bye from the second client
Server replied: bye from the second client
Enter the message to send to server (type 'exit' to quit):
exit
Client exiting...
```
