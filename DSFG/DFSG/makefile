OBJS= serialDFS ompDFS

all:$(OBJS)

serialDFS:serialDFS.o
	g++ -o serialDFS serialDFS.o
ompDFS:
	g++ -o ompDFS ompDFS.cpp -fopenmp

clean:
	rm -rf *o $(OBJS)
