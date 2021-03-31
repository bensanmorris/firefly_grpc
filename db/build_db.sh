#!/usr/bin/sh
scriptDir=$(dirname "$0")
db_user=root
db_pwd=

echo "building database..."
mysql --user=$db_user --password=$db_pwd < ./db.sql
