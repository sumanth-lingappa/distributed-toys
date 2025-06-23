# Sharding Demo

```bash
❯ docker compose up --build -d
❯ curl -X POST -H "Content-Type: application/json" -d '{"user_id": 3, "name": "Alice"}' http://localhost:5050/users

❯ curl -X POST -H "Content-Type: application/json" -d '{"user_id": 1, "name": "Ravi"}'   http://localhost:5050/users
curl -X POST -H "Content-Type: application/json" -d '{"user_id": 2, "name": "Meena"}'  http://localhost:5050/users
curl -X POST -H "Content-Type: application/json" -d '{"user_id": 3, "name": "Alice"}'  http://localhost:5050/users
curl -X POST -H "Content-Type: application/json" -d '{"user_id": 4, "name": "Kiran"}'  http://localhost:5050/users
curl -X POST -H "Content-Type: application/json" -d '{"user_id": 5, "name": "John"}'   http://localhost:5050/users

❯ docker exec -it mysql-shard1 mysql -uroot -prootpass -e "SELECT * FROM users.user;"
mysql: [Warning] Using a password on the command line interface can be insecure.
+---------+-------+
| user_id | name  |
+---------+-------+
|       2 | Meena |
|       4 | Kiran |
+---------+-------+
❯ docker exec -it mysql-shard2 mysql -uroot -prootpass -e "SELECT * FROM users.user;"
mysql: [Warning] Using a password on the command line interface can be insecure.
+---------+-------+
| user_id | name  |
+---------+-------+
|       1 | Ravi  |
|       3 | Alice |
|       5 | John  |
+---------+-------+
```
