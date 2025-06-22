CREATE DATABASE IF NOT EXISTS replica_test_db;

CREATE USER IF NOT EXISTS 'repl'@'%' IDENTIFIED WITH mysql_native_password BY 'replpass';
GRANT REPLICATION SLAVE ON *.* TO 'repl'@'%';
FLUSH PRIVILEGES;
USE replica_test_db;
CREATE TABLE messages (id INT AUTO_INCREMENT PRIMARY KEY, text VARCHAR(255));
INSERT INTO messages (text) VALUES ('Auto-replication works!');
