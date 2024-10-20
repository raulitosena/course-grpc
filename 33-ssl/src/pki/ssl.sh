#!/bin/bash

echo "Generating Private Keys..."
openssl genpkey -algorithm RSA -out root.key -pkeyopt rsa_keygen_bits:2048
openssl genpkey -algorithm RSA -out server.key -pkeyopt rsa_keygen_bits:2048
openssl genpkey -algorithm RSA -out client.key -pkeyopt rsa_keygen_bits:2048

echo "Generating Certificate Signing Request..."
openssl req -new -key root.key -out root.csr -config root.conf
openssl req -new -key server.key -out server.csr -config server.conf
openssl req -new -key client.key -out client.csr -config client.conf

echo "Signing certificates..."
openssl x509 -req -days 365 -in root.csr -signkey root.key -out root.crt
openssl x509 -req -in server.csr -CA root.crt -CAkey root.key -CAcreateserial -out server.crt -days 365 -sha256
openssl x509 -req -in client.csr -CA root.crt -CAkey root.key -CAcreateserial -out client.crt -days 365 -sha256
