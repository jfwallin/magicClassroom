import socket
import time

UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 9999 
Message = "Hello, Server"

clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

for i in range(100):
    mm = Message + str(i)
    print mm
    clientSock.sendto(mm, (UDP_IP_ADDRESS, UDP_PORT_NO))
    time.sleep(1)
    


