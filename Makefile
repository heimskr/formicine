COMPILER		= clang++
CC				= $(COMPILER) -std=c++17 -g -O3 -Wall -Wextra
OBJECTS			= ansi.o
TESTOUTPUT		= ansi

all: $(TESTOUTPUT)

test: $(TESTOUTPUT)
	./$(TESTOUTPUT)

$(TESTOUTPUT): test.o $(OBJECTS)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $<

clean:
	rm -f *.o $(TESTOUTPUT)
