CC := g++
CFLAGS := -std=c++11
exe = hw3

all:main.cpp problem.hpp ILP.hpp checker.hpp
	@$(CC) main.cpp -o $(exe) $(CFLAGS)

clean:
	@rm -f *.o $(exe) *.lp solution.txt

