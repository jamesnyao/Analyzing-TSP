G++ = g++ -std=c++11

all: TSP

TSP: Driver.o TSP_Functions.o
	$(G++) -o TSP Driver.o TSP_Functions.o

Driver.o: Driver.cpp
	$(G++) -c Driver.cpp

TSP_Functions.o: TSP_Functions.cpp TSP_Functions.h
	$(G++) -c TSP_Functions.cpp

clean:
	rm -vf TSP *.o
	
run: TSP
	./TSP
	