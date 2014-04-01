
# Default Flags
FLAGS = -Wall -Wextra -pedantic -Wvla -std=c++11 -O3

# make release - will compile "all" with $(FLAGS) and the O3 flag
release:
release: all

all: parser.o virtual_machine.o eloquent.o
	g++ $(FLAGS) eloquent.o parser.o virtual_machine.o -o eloquent

eloquent.o: eloquent.cpp parser
	g++ $(FLAGS) -c eloquent.cpp

parser.o: parser
	g++ -c parser.cpp

virtual_machine.o: virtual_machine.cpp
	g++ $(FLAGS) -c virtual_machine.cpp

parser:
	lemon parser.y -s
	mv parser.c parser.cpp

clean:
	rm -f *.o
	rm -f eloquent
	rm -f parser.cpp
	rm -f parser.h
	rm -f parser.out