from flask import Flask, render_template
from flask_socketio import SocketIO, send

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app,
                    cors_allowed_origins='*',
                    ping_interval=5, # every 5s, cient sends "2" ping
                    ping_timeout=60, # server will close connection if no "3"(pong) in 60s
                    async_mode="eventlet")

@app.route('/')
def index():
    return render_template('index.html')

@socketio.on('message')
def handle_message(msg):
    print(f"Received message: {msg}")
    send(msg, broadcast=True)

if __name__ == "__main__":
    socketio.run(app, host='0.0.0.0', port=5050)
