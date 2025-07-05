import logging

from flask import Flask, request, send_file
import requests, os

logging.basicConfig(level=logging.DEBUG)

app = Flask(__name__)
ORIGIN="http://origin:9000"
EDGE_CACHE_DIR = "./cache"

os.makedirs(EDGE_CACHE_DIR, exist_ok=True)

@app.route("/content/<filename>")
def serve_file(filename):
    path = f"{EDGE_CACHE_DIR}/{filename}"
    if os.path.exists(path):
        logging.info(f"CACHE HIT: {filename}")
        return send_file(path)
    logging.info(f"CACHE MISS: {filename} - fetching from origin...")
    r = requests.get(f"{ORIGIN}/content/{filename}")
    if r.status_code == 200:
        with open(path, "wb") as f:
            f.write(r.content)
        logging.info(f"Fetched from origin and cached: {filename}")
        return r.content
    logging.warning(f"Origin returned {r.status_code} for {filename}")
    return "File Not Found", 404

app.run(host="0.0.0.0", port=8000)