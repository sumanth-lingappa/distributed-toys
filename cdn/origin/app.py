import logging
import os
from flask import Flask, send_file

logging.basicConfig(level=logging.DEBUG)

app = Flask(__name__)
ORIGIN_DATA_DIR = "./origin_data"

@app.route("/content/<filename>")
def get_file(filename):
    filepath = os.path.join(ORIGIN_DATA_DIR, filename)
    if os.path.exists(filepath):
        logging.info(f"Origin served file: {filename}")
        return send_file(f"./origin_data/{filename}")
    logging.warning(f"Origin missing file: {filename}")
    return "Not Found", 404

app.run(host="0.0.0.0", port=9000)
