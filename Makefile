COMPILER		:= clang++
CC				:= $(COMPILER) -std=c++17 -g -O3 -Wall -Wextra
OBJECTS			:= ansi.o
TESTOUTPUT		:= ansi

ifeq ($(shell uname -s), Darwin)
	SDKFLAGS	:= --sysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
endif

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
