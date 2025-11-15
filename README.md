# Encrypted Patterns

## Project Description

**Encrypted Patterns** is a client-server demo written in C that transmits encrypted pattern data from a client to a server. It demonstrates SHA-256 hashing & AES-128-CBC encryption, TCP sockets, and command-line interface handling with getopt.

## Dependencies

- Run on a Linux machine or VM with `gcc` & `cmake` installed.
- Install the latest version of OpenSSL:

```bash
sudo apt install openssl
```

## Usage

1. Clone the repository.
2. In this directory, run `make` (or use the provided executables).
3. In the `keys` folder, run `bash genkeys.sh` (or use the pre-generated keys).
4. In one terminal, run the server using the instructions in [Server Usage](#server).
5. In another terminal, run the client using the instructions in [Client Usage](#client). An example file, `pattern1.txt`, is provided for the `-f` option.


## Server

### Command:

>./server -t PORT
            
### Description:
Recieve an encrypted 4 byte message pattern to a server.

### Arguments:
| **Arg**  | **Opt** | **Desc** |
| :--- | :---: | ---: |
|-t| PORT |set the port for TCP|

### Example:

```bash
./server -t 8001
```

## Client

### Command:

>./client [-h] [-v] (-f FILE | -p VALUE,TIME)

### Description:
Send an encrypted 4 byte message pattern to a server.

### Arguments:

| **Arg** | **Opt** | **Description** |
| :--- | :---: | ---: |
|-h| |show this help message and exit|
|-v| |enable verbose output|
|-p|VALUE(hex),TIME(ms)|specify pattern, can repeat for a sequence.|
|-f|FILE|specify a text file with \<VALUE> \<TIME> lines|

### Example:

```bash
./client -v -f pattern1.txt
./client -p 0x01,200 -p 0xff,500 -p 0x32,200
```

## Flowchart

![Flowchart](assets/flowchart.png)

## Acknowledgments 
- **Language**: C
- **Tools** Getopt, OpenSSL