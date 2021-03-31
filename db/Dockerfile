FROM mariadb:latest

MAINTAINER Ben Morris <bengrpc_cmake.fireflytech.org>

ENV MYSQL_ROOT_PASSWORD=root

COPY db.sql /docker-entrypoint-initdb.d/db.sql

RUN chmod a+rwx /docker-entrypoint-initdb.d/db.sql

RUN ls -la /docker-entrypoint-initdb.d/

EXPOSE 3306
