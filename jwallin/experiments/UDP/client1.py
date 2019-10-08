import socket

UDP_IP_ADDRESS = "127.0.0.1"
UDP_IP_ADDRESS = "161.45.205.254"
UDP_PORT_NO = 6789
Message = "Hello, Server"

clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSock.sendto(Message, (UDP_IP_ADDRESS, UDP_PORT_NO))


