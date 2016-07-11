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
        fromServer = sock.recv(100).decode()
        if fromServer == "EXIT":
            print("server requested emergency exit!")
            exit()
        else:
            return fromServer
    else:
        sock.send((msg + "\n\0").encode())
        time.sleep(1)


#game stuff 
output("Welcome to cobweb MUD!", False)
output("Press enter to receive helmet of safety :3", True)
output("GIVE0/4", False)
output("Type 'list inv' to view inventory", True)
output("Nice job! Press enter to exit game", True)
sock.send("<EXIT>\0".encode())
sock.close()
exit()
