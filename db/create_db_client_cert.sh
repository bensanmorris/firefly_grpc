#!/bin/sh
scriptDir=$(dirname "$0")
if [ -f "/var/lib/mysql/client-cert.pem" ]; then
    sudo cat /var/lib/mysql/client-cert.pem /var/lib/mysql/client-key.pem > $scriptDir/../db_client_cert.pem
    sudo cp /var/lib/mysql/ca.pem $scriptDir/../db_ca.pem
fi
if [ -f "/etc/mysql/ssl/client-cert.pem" ]; then
    sudo cat /etc/mysql/ssl/client-cert.pem /etc/mysql/ssl/client-key.pem > $scriptDir/../db_client_cert.pem
    sudo cp /etc/mysql/ssl/ca-cert.pem $scriptDir/../db_ca.pem
fi

