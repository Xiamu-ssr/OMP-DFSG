OBJS= handleInput handleInputSort handleDict handleRes dataCheckWithShared

all:$(OBJS)

handleInputSort:
	g++ -o handleInputSort handleInputSort.cpp -fopenmp
handleInput:
	g++ -o handleInput handleInput.cpp -fopenmp
handleDict:
	g++ -o handleDict handleDict.cpp -fopenmp
handleRes:
	g++ -o handleRes handleRes.cpp -fopenmp
dataCheckWithShared:
	g++ -o dataCheckWithShared dataCheckWithShared.cpp -fopenmp

clean:
	rm -rf *o $(OBJS)
