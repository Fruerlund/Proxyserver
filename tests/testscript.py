import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("127.0.0.1", 5555))

s.send(b"\x04\x01\x05\x05\x00\x00\x00\x01Fruerlund\x00www.example.com\x00")
print(s.recv(1024))

while(True):
	
	s.send(input().encode())
