CC := g++
CFLAGS := -g -std=c++11
exe = hw4

all:main.cpp truth_table.hpp utils.hpp operators.hpp cofactors.hpp
	@$(CC) main.cpp -o $(exe) $(CFLAGS)

clean:
	@rm -rf *.o *.dSYM $(exe)

