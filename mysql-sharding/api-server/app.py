from flask import Flask, request, jsonify
import mysql.connector

app = Flask(__name__)

# Simple shard map: user_id % 2
def get_shard_connection(user_id):
    shard = user_id % 2
    config = {
        0: {"host": "mysql-shard1", "port": 3306},
        1: {"host": "mysql-shard2", "port": 3306},
    }[shard]

    return mysql.connector.connect(
        host=config["host"],
        port=config["port"],
        user="root",
        password="rootpass",
        database="users"
    )

@app.route("/users", methods=["POST"])
def create_user():
    data = request.get_json()
    user_id = int(data["user_id"])
    name = data["name"]

    conn = get_shard_connection(user_id)
    cursor = conn.cursor()
    cursor.execute("INSERT INTO user (user_id, name) VALUES (%s, %s)", (user_id, name))
    conn.commit()
    cursor.close()
    conn.close()
    return jsonify({"message": "User inserted into shard"}), 201

@app.route("/users/<int:user_id>", methods=["GET"])
def get_user(user_id):
    conn = get_shard_connection(user_id)
    cursor = conn.cursor()
    cursor.execute("SELECT user_id, name FROM user WHERE user_id = %s", (user_id,))
    row = cursor.fetchone()
    cursor.close()
    conn.close()

    if row:
        return jsonify({"user_id": row[0], "name": row[1]}), 200
    else:
        return jsonify({"error": "User not found"}), 404

if __name__ == "__main__":
    app.run(host="0.0.0.0")