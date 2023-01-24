all : puyo_client puyo_server

puyo_client : puyo.cpp
	g++ -o ./app/puyo_client puyo.cpp -D_REENTRANT -lncurses -lpthread
puyo_server : puyo_server.c
	gcc -o ./server/puyo_server puyo_server.c -D_REENTRANT -lpthread
clean:
	rm ./app/puyo_client ./server/puyo_server
