-- Ensure the database exists before starting replication
CREATE DATABASE IF NOT EXISTS replica_test_db;

-- Optional table creation if replication missed it (non-ideal for prod but safe here)
USE replica_test_db;

CREATE TABLE IF NOT EXISTS messages (
    id INT AUTO_INCREMENT PRIMARY KEY,
    text VARCHAR(255)
);

-- Small wait to ensure master is up (optionally replace with WAIT-FOR logic)
DO SLEEP(10);

CHANGE REPLICATION SOURCE TO
  SOURCE_HOST='mysql-master',
  SOURCE_USER='repl',
  SOURCE_PASSWORD='replpass',
  SOURCE_LOG_FILE='mysql-bin.000003',
  SOURCE_LOG_POS=157,
  GET_SOURCE_PUBLIC_KEY = 1;

START REPLICA;