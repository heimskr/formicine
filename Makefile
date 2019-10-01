COMPILER		:= g++
CC				:= $(COMPILER) -std=c++2a -g -O0 -Wall -Wextra
OBJECTS			:= ansi.o futil.o
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
	$(CC) $(SDKFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(SDKFLAGS) -c $<

clean:
	rm -f *.o $(TESTOUTPUT)
