import socket
import sys

#server stuff
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
serverP = "gameScripts/.sock" + sys.argv[1]
print(serverP.encode())
sock.connect(serverP)

def uIn():
    msg = sock.recv(100).decode()
    return msg
    

def output(msg):
    sock.send((msg + "\n\0").encode())

#game stuff 
output("whats your name")
name = uIn()
output("hia " + name + ". Press enter to exit game")
uIn()
sock.send("<EXIT>\0".encode())
print("done!")
