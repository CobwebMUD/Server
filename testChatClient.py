#chatClient.py <- sample chat room client

import socket
import _thread as thread

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 8575))

def listen():
    while (True):
        print(sock.recv(128).decode())

thread.start_new_thread(listen, ())

while (True):
    msg = input()
    sock.send(msg.encode())
    if msg == '':
        break
