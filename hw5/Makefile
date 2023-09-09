CC := g++
CFLAGS := -g -std=c++11
exe = hw5

all:main.cpp BDD.hpp truth_table.hpp operations.hpp
	@$(CC) main.cpp -o $(exe) $(CFLAGS)

clean:
	@rm -rf *.o *.dSYM $(exe)

