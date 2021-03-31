DROP DATABASE IF EXISTS `_test_db`;
CREATE DATABASE `_test_db`;

USE `_test_db`;
DROP TABLE IF EXISTS `server`;
CREATE TABLE `server` (
    address VARCHAR(255) CHARACTER SET 'latin1' NOT NULL,
    port INT NOT NULL,
    environment TEXT NOT NULL,
    certificate TEXT NOT NULL,
    created_timestamp TIMESTAMP NOT NULL,
    checkin_timestamp TIMESTAMP NOT NULL,
    PRIMARY KEY(address, port)
);

DROP TABLE IF EXISTS `session`;
CREATE TABLE `session` (
    session_id INT NOT NULL AUTO_INCREMENT,
    caller_identity TEXT NOT NULL,
    created_timestamp TIMESTAMP NOT NULL,
    PRIMARY KEY(session_id)
);

DROP TABLE IF EXISTS `location`;
CREATE TABLE `location` (
    session_id INT NOT NULL,
    x DOUBLE NOT NULL,
    y DOUBLE NOT NULL,
    z DOUBLE NOT NULL,
    user_data TEXT
);

#GRANT USAGE ON *.* TO 'ff_user'; # workaround for no IF EXISTS supported on DROP user for mysql versions < 5.7
#DROP USER 'ff_user';
CREATE USER 'ff_user' IDENTIFIED BY 'Test123!';
GRANT SELECT, INSERT, UPDATE, DELETE ON server TO 'ff_user';
GRANT SELECT, INSERT, UPDATE, DELETE ON session TO 'ff_user';
GRANT SELECT, INSERT, UPDATE, DELETE ON location TO 'ff_user';
