CC := g++
CFLAGS := -std=c++11
exe = hw0
exe_adv = hw0_adv

all:main.cpp graph.hpp
	@$(CC) main.cpp -o $(exe) $(CFLAGS)

adv:main_adv.cpp directed_graph.hpp
	@$(CC) main_adv.cpp -o $(exe_adv) $(CFLAGS)

clean:
	@rm -f *.o $(exe) $(exe_adv)

