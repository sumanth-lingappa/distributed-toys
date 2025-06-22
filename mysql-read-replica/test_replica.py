import mysql.connector

# Connection config
master_config = {
    "host": "localhost",  # or "127.0.0.1" if port mapped
    "port": 3306,         # map this via docker if needed
    "user": "root",
    "password": "rootpass",
    "database": "replica_test_db"
}

replica_config = {
    "host": "localhost",  # or "127.0.0.1"
    "port": 3307,         # assume we'll map this separately
    "user": "root",
    "password": "rootpass",
    "database": "replica_test_db"
}

# INSERT into master
def insert_into_master():
    conn = mysql.connector.connect(**master_config)
    cursor = conn.cursor()
    cursor.execute("INSERT INTO messages (text) VALUES ('Inserted from Python!')")
    conn.commit()
    cursor.close()
    conn.close()
    print("✅ Inserted into master.")

# READ from replica
def read_from_replica():
    conn = mysql.connector.connect(**replica_config)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM messages")
    rows = cursor.fetchall()
    cursor.close()
    conn.close()
    print("✅ Read from replica:")
    for row in rows:
        print(row)

if __name__ == "__main__":
    insert_into_master()
    read_from_replica()