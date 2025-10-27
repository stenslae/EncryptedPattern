# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Final executables
all: server client

# Compile source to object files
server: server.o pattern.o
	$(CC) $(CFLAGS) server.o printar.o -o server

client: client.c pattern.o
	$(CC) $(CFLAGS) client.c pattern.o -o client

# Compile source to object files
server.o: server.c pattern.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c pattern.h
	$(CC) $(CFLAGS) -c client.c

pattern.o: pattern.c pattern.h
	$(CC) $(CFLAGS) -c pattern.c

# Clean up compiled files
clean:
	rm -f *.o