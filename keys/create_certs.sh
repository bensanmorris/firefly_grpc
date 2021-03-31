#!/bin/bash

# usage: ./create_certs.sh [certificate authority name] e.g. ./create_certs.sh MyCA
set -eux

# remove any existing certificates prior to running
scriptDir=$(dirname "$0")
echo "$scriptDir"
rm -f $scriptDir/*.p12
rm -f $scriptDir/*.der
rm -f $scriptDir/*.pem

# holds the CA name used to create the root cert
ca_subject="/CN=${1}"

# create the CA keypair and a self-signed certificate.
openssl genrsa -out $scriptDir/ca-key.pem 2048
openssl req -new \
    -sha256 \
    -subj "$ca_subject" \
    -key $scriptDir/ca-key.pem \
    -out $scriptDir/ca-csr.pem
openssl x509 -req \
    -sha256 \
    -signkey $scriptDir/ca-key.pem \
    -extensions a \
    -extfile <(echo '[a]
        basicConstraints=critical,CA:TRUE,pathlen:0
    ') \
    -days 3650 \
    -in  $scriptDir/ca-csr.pem \
    -out $scriptDir/ca-crt.pem
openssl x509 -outform der -in $scriptDir/ca-crt.pem -out $scriptDir/ca-crt.der

# our server host names
servers=(
    'localhost'
)

# our server service names
service_names=(
    'server.fireflytech.org'
)

# create the server keypairs and their certificates signed by the test CA.
counter=0
for server in ${servers[@]}; do
    openssl genrsa \
        -out $scriptDir/$server-server-key.pem \
        2048 \
        2>/dev/null
    openssl req -new \
        -sha256 \
        -subj "/CN=$server/OU=${service_names[$counter]}" \
        -key $scriptDir/$server-server-key.pem \
        -out $scriptDir/$server-server-csr.pem
    openssl x509 -req -sha256 \
        -CA $scriptDir/ca-crt.pem \
        -CAkey $scriptDir/ca-key.pem \
        -set_serial 1 \
        -extensions a \
        -extfile <(echo "[a]
            subjectAltName=DNS:$server
            extendedKeyUsage=serverAuth,clientAuth
	        basicConstraints = critical,CA:FALSE
            ") \
        -days 3650 \
        -in  $scriptDir/$server-server-csr.pem \
        -out $scriptDir/$server-server-crt.pem
    openssl x509 -outform der -in $scriptDir/$server-server-crt.pem -out $scriptDir/$server-server-crt.der
    openssl pkcs12 -export \
        -inkey $scriptDir/$server-server-key.pem \
        -in    $scriptDir/$server-server-crt.pem \
        -out   $scriptDir/$server-server.p12 \
        -passout pass:

    # create certificate chain file
    cat $scriptDir/$server-server-crt.pem $scriptDir/$server-server-key.pem $scriptDir/ca-crt.pem > $scriptDir/$server-server.pem
    counter=$((counter + 1))
done

# our client host names
clients=(
    'localhost'
)

# our client key domains
key_domains=(
    'broker_client_keys'
)

# create the client keypairs and their certificates signed by the test CA.
counter=0
for client in ${clients[@]}; do
    openssl genrsa \
        -out $scriptDir/$client-client-key.pem \
        2048 \
        2>/dev/null
    openssl req -new \
        -sha256 \
        -subj "/CN=$client/OU=${key_domains[$counter]}" \
        -key $scriptDir/$client-client-key.pem \
        -out $scriptDir/$client-client-csr.pem
    openssl x509 -req -sha256 \
        -CA $scriptDir/ca-crt.pem \
        -CAkey $scriptDir/ca-key.pem \
        -set_serial 1 \
        -extensions a \
        -extfile <(echo "[a]
            subjectAltName=DNS:$client
            extendedKeyUsage=serverAuth,clientAuth
	    basicConstraints = critical,CA:FALSE
            ") \
        -days 3650 \
        -in  $scriptDir/$client-client-csr.pem \
        -out $scriptDir/$client-client-crt.pem
    openssl x509 -outform der -in $scriptDir/$client-client-crt.pem -out $scriptDir/$client-client-crt.der
    openssl pkcs12 -export \
        -inkey $scriptDir/$client-client-key.pem \
        -in    $scriptDir/$client-client-crt.pem \
        -out   $scriptDir/$client-client.p12 \
        -passout pass:

    # create certificate chain file
    cat $scriptDir/$client-client-crt.pem $scriptDir/$client-client-key.pem $scriptDir/ca-crt.pem > $scriptDir/$client-client.pem
    counter=$((counter + 1))
done

# rename CA certificate file
mv $scriptDir/ca-crt.pem $scriptDir/"${1}".pem

