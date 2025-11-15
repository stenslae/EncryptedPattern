# Encrypted Patterns

## Project Description

**Encrypted Patterns** is a client-server demo written in C that transmits pattern data over TCP using the OpenSSL's cryptography library. It demonstrates RSA & AES encryption, socket programming, and command-line interface handling with getopt.

## Flowchart

![Flowchart](assets/flowchart.png)

## Dependencies

## Installation
1. Clone this repo on a machine that's running a Linux distro.
2. Install all OpenSSL dependencies:

```bash
sudo studio apt install
sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt install openssl
sudo apt-get install libssl-dev:armhf
```

3. In this directory, run make to get the executable for the ARM CPU using the following:

```bash
make CC=arm-linux-gnueabihf-gcc
```

4. Copy this compiled file into the mounted NFS Kernel's home using the following:

```bash
cp encrypt /srv/nfs/de10nano/nfs-kernel-server/home/soc/
```

5. Run `server` in one terminal, using the instructions in [Server Usage](#server-usage)
6. Run `client` in another terminal, using the instructions in [Client Usage](#client-usage). An example file, [pattern1.txt](assets/pattern1.txt) for -f is provided.

## Server Usage
### Usage:
./server -t PORT
            
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

## Client Usage
### Usage:
./client [-h] [-v] ([-f FILE] | [-p VALUE,TIME])

### Description:
Send an encrypted 4 byte message pattern to a server.

### Arguments:

| **Arg**  | **Opt** | **Description** |
| :--- | :---: | ---: |
|-h| |show this help message and exit|
|-v| |enable verbose output|
|-p|VALUE,TIME|specify pattern with VALUE (hex) and TIME (ms). can be used multiple times to create a sequence.|
|-f|FILE|specify a text file with \<VALUE> \<TIME> lines|

### Example:

![Example Output](assets/example_output.png)

## Acknowledgments 
- **Language and Tools**: C
- **Tools** Getopt, OpenSSL