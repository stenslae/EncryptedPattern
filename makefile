# Compiler and flags
CC ?= gcc
CFLAGS ?= -Wall -g
OPENSSL = -lssl -lcrypto

# Final executables
all: server client

# Compile source to object files
server: server.o pattern.o
	$(CC) $(CFLAGS) server.o pattern.o encrypt.o -o server $(OPENSSL)

client: client.c pattern.o
	$(CC) $(CFLAGS) client.c pattern.o encrypt.o -o client $(OPENSSL)

# Compile source to object files
server.o: server.c pattern.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c pattern.h
	$(CC) $(CFLAGS) -c client.c

pattern.o: pattern.c pattern.h
	$(CC) $(CFLAGS) -c pattern.c

encrypt.o: encrypt.c encrypt.h
	$(CC) $(CFLAGS) -c encrypt.c

# Clean up compiled files
clean:
	rm -f *.o