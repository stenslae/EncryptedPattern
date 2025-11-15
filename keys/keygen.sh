#!/bin/bash

# Generate the private key & public key using ecliptic curve
echo "Generating EC private key & public key pair..."
openssl ecparam -genkey -name secp128r2 -noout -out private_key.pem
openssl ec -in private_key.pem -pubout -out public_key.pem

# Hash the public key
echo "Hashing the public key..."
openssl dgst -sha256 public_key.pem > public_key_hash.txt

# Hash the client
echo "Hashing the client..."
openssl dgst -sha256 ../client  | openssl base64 > client_hash.txt

# Store the public key hash in a file
openssl dgst -sha256 -binary public_key.pem | openssl base64 > public_key_hash.txt

# Sign client with private key
echo "Signing client..."
openssl dgst -sha256 -sign private_key.pem -out client.sig ../client

echo "Completed key & signature generation!"
