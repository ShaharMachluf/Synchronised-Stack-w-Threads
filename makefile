CC=g++
STACK=stack.o
SERVER=server.o
CLIENT=client.o
FLAGS=-g

all: server client tests
tests: tests.o server
	$(CC) $(FLAGS) -o tests tests.o
part5: part5.o $(STACK)
	$(CC) $(FLAGS) -o part5 part5.o
server: $(SERVER) $(STACK) part8.o
	$(CC) $(FLAGS) -pthread -o server $(SERVER)
client: $(CLIENT) $(STACK)
	$(CC) $(FLAGS) -o client $(CLIENT)
part5.o: $(STACK) part5.cpp
	$(CC) $(FLAGS) -c part5.cpp
$(SERVER): server.cpp
	$(CC) $(FLAGS) -c server.cpp 
$(CLIENT): client.cpp
	$(CC) $(FLAGS) -c client.cpp 
$(STACK): stack.cpp
	$(CC) $(FLAGS) -c stack.cpp 
part8.o: part8.cpp
	$(CC) $(FLAGS) -c part8.cpp 
tests.o: tests.cpp
	$(CC) $(FLAGS) -c tests.cpp 

.PHONY: clean all
clean:
	rm -f *.o part5 server client tests
