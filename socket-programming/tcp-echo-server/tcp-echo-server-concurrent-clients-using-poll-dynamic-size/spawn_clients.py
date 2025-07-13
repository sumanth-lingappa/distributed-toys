import socket
import time

clients = []

HOST = "127.0.0.1"
SERVER_PORT = 8080
NUM_CLIENTS = 50000


for i in range(NUM_CLIENTS):
    try:
        s = socket.socket()
        s.connect((HOST, SERVER_PORT))
        clients.append(s)
        print(f"[{i}] Connected")
        time.sleep(0.002)  # small delay to spread out load
    except Exception as e:
        print(f"[{i}] Failed: {e}")
        break

print(f"Total connected clients: {len(clients)}")

input("Press enter to close all clients...")
for s in clients:
    s.close()
