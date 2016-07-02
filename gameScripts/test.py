import socket
import sys
import time

# connect to server
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
serverP = "gameScripts/.sock" + sys.argv[1]
sock.connect(serverP)

# @param string msg, bool RSVP <- true if response expected from User false otherwise 
def output(msg, RSVP):
    if RSVP == True:
        sock.send(("RSVP" + msg + "\n\0").encode())
        return sock.recv(100).decode()
    else:
        sock.send((msg + "\n\0").encode())
        time.sleep(1)


#game stuff 
name = output("whats your name", True)
output("hia " + name + "!!!", False)
output("Press enter to exit game", True)
sock.send("<EXIT>\0".encode())
sock.close()
exit()
