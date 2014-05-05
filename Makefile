# Default Flags
FLAGS = -Wall -Wextra -pedantic -Wvla -std=c++11 -O3

# make release - will compile "all" with $(FLAGS) and the O3 flag
release:
release: all

all: lexer.o value.o scanner.o parser.o virtual_machine.o compiler.o eloquent.o
	g++ $(FLAGS) eloquent.o value.o lexer.o scanner.o parser.o virtual_machine.o compiler.o -o eloquent

eloquent.o: eloquent.cpp
	g++ $(FLAGS) -c eloquent.cpp

virtual_machine.o: virtual_machine.cpp
	g++ $(FLAGS) -c virtual_machine.cpp

compiler.o: compiler.cpp
	g++ $(FLAGS) -c compiler.cpp

parser.o: parser.cpp scanner
	g++ $(FLAGS) -c parser.cpp

lexer.o: lexer.cpp scanner
	g++ $(FLAGS) -c lexer.cpp 

scanner.o: scanner
	g++ -c scanner.cpp

scanner:
	lemon scanner.y -s
	mv scanner.c scanner.cpp

value.o: value.cpp
	g++ $(FLANGS) -c value.cpp

clean:
	rm -f *.o
	rm -f eloquent
	rm -f scanner.cpp
	rm -f scanner.h
	rm -f scanner.out