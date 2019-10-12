COMPILER		:= g++
CC				:= $(COMPILER) -std=c++2a -g -O0 -Wall -Wextra
OBJECTS			:= ansi.o futil.o
TESTOUTPUT		:= ansi

ifeq ($(CHECK), asan)
	CHECKFLAGS := -fsanitize=address -fno-common
else ifeq ($(CHECK), msan)
	CHECKFLAGS := -fsanitize=memory -fno-common
endif

all: $(TESTOUTPUT)

test: $(TESTOUTPUT)
	./$(TESTOUTPUT)

$(TESTOUTPUT): test.o $(OBJECTS)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $<

clean:
	rm -f *.o $(TESTOUTPUT)
