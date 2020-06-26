all:
	g++ serverMultithread.cpp -g -o server.exe -pthread -std=c++11
	g++ clientMultithread.cpp -g -o client.exe -pthread -std=c++11

s:
	./server.exe

c:
	./client.exe

clean:
	rm *.exe