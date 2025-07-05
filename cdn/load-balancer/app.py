import logging

from flask import Flask, redirect
import random

logging.basicConfig(level=logging.DEBUG)

app = Flask(__name__)
EDGE_SERVERS = (
    "http://localhost:8001",
    "http://localhost:8002",
)


@app.route("/content/<filename>")
def redirect_to_edge(filename):
    edge = random.choice(EDGE_SERVERS)
    logging.info(f"FORWARDING request for '{filename} to edge: {edge}")
    return redirect(f"{edge}/content/{filename}")

app.run(host="0.0.0.0", port=7000)