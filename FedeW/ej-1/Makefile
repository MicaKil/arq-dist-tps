.PHONY: build clean run

run: build
	./main.o
build: main.o

main.o: main.cpp
	mpic++ main.cpp -o main.o -lstdc++

main.i: main.cpp
	mpic++ -E main.cpp -o main.i -lstdc++

clean:
	rm main.o
