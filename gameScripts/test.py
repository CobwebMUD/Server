import socket
import sys
import time

#server stuff
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
serverP = "gameScripts/.sock" + sys.argv[1]
sock.connect(serverP)

def uIn():
    msg = sock.recv(100).decode()
    return msg
    

# @param string msg, bool RSVP <- true if response expected from User false otherwise 
def output(msg, RSVP):
    if RSVP == True:
        sock.send(("RSVP" + msg + "\n\0").encode())
    else:
        sock.send((msg + "\n\0").encode())
        time.sleep(1)
        print("wooting right now!")


#game stuff 
output("whats your name", True)
name = uIn()
output("hia " + name + "!!!", False)
output("Press enter to exit game", True)
uIn()
sock.send("<EXIT>\0".encode())
sock.close()
exit()
