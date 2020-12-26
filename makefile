all : puyo_client puyo_server

puyo_client : puyo.cpp
	g++ -o puyo_client puyo.cpp -D_REENTRANT -lncurses -lpthread
puyo_server : puyo_server.c
	gcc -o puyo_server puyo_server.c -D_REENTRANT -lpthread
clean:
	rm puyo_client puyo_server *.o
