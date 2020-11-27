all	: puyo.o
	g++ -o main puyo.cpp -lncurses -lpthread
clean:
	rm main *.o
