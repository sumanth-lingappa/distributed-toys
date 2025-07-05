# CDN

## How to run

```bash
docker compose up --build -d
```

## Execute

Execute the below 4 times for cache misses and hits

```bash
curl -vL  http://localhost:7100/content/sample.jpg --output out.jpg
```

## Logs

```bash
❯ docker compose logs -f
edge1          |  * Serving Flask app 'app'
load-balancer  |  * Serving Flask app 'app'
edge2          |  * Serving Flask app 'app'
load-balancer  |  * Debug mode: off
edge1          |  * Debug mode: off
edge1          | INFO:werkzeug:WARNING: This is a development server. Do not use it in a production deployment. Use a production WSGI server instead.
edge2          |  * Debug mode: off
edge1          |  * Running on all addresses (0.0.0.0)
edge1          |  * Running on http://127.0.0.1:8000
origin         |  * Serving Flask app 'app'
load-balancer  | INFO:werkzeug:WARNING: This is a development server. Do not use it in a production deployment. Use a production WSGI server instead.
origin         |  * Debug mode: off
load-balancer  |  * Running on all addresses (0.0.0.0)
edge2          | INFO:werkzeug:WARNING: This is a development server. Do not use it in a production deployment. Use a production WSGI server instead.
edge2          |  * Running on all addresses (0.0.0.0)
edge2          |  * Running on http://127.0.0.1:8000
load-balancer  |  * Running on http://127.0.0.1:7000
load-balancer  |  * Running on http://192.168.96.5:7000
load-balancer  | INFO:werkzeug:Press CTRL+C to quit
edge1          |  * Running on http://192.168.96.4:8000
edge2          |  * Running on http://192.168.96.3:8000
edge1          | INFO:werkzeug:Press CTRL+C to quit
origin         | INFO:werkzeug:WARNING: This is a development server. Do not use it in a production deployment. Use a production WSGI server instead.
origin         |  * Running on all addresses (0.0.0.0)
origin         |  * Running on http://127.0.0.1:9000
origin         |  * Running on http://192.168.96.2:9000
edge2          | INFO:werkzeug:Press CTRL+C to quit
origin         | INFO:werkzeug:Press CTRL+C to quit



load-balancer  | INFO:root:FORWARDING request for 'sample.jpg to edge: http://localhost:8002
load-balancer  | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:10:43] "GET /content/sample.jpg HTTP/1.1" 302 -
edge2          | INFO:root:CACHE MISS: sample.jpg - fetching from origin...
edge2          | DEBUG:urllib3.connectionpool:Starting new HTTP connection (1): origin:9000
origin         | INFO:root:Origin served file: sample.jpg
origin         | INFO:werkzeug:192.168.96.3 - - [05/Jul/2025 18:10:43] "GET /content/sample.jpg HTTP/1.1" 200 -
edge2          | DEBUG:urllib3.connectionpool:http://origin:9000 "GET /content/sample.jpg HTTP/1.1" 200 29428
edge2          | INFO:root:Fetched from origin and cached: sample.jpg
edge2          | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:10:43] "GET /content/sample.jpg HTTP/1.1" 200 -


load-balancer  | INFO:root:FORWARDING request for 'sample.jpg to edge: http://localhost:8001
load-balancer  | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:10:51] "GET /content/sample.jpg HTTP/1.1" 302 -
edge1          | INFO:root:CACHE MISS: sample.jpg - fetching from origin...
edge1          | DEBUG:urllib3.connectionpool:Starting new HTTP connection (1): origin:9000
origin         | INFO:root:Origin served file: sample.jpg
origin         | INFO:werkzeug:192.168.96.4 - - [05/Jul/2025 18:10:51] "GET /content/sample.jpg HTTP/1.1" 200 -
edge1          | DEBUG:urllib3.connectionpool:http://origin:9000 "GET /content/sample.jpg HTTP/1.1" 200 29428
edge1          | INFO:root:Fetched from origin and cached: sample.jpg
edge1          | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:10:51] "GET /content/sample.jpg HTTP/1.1" 200 -



load-balancer  | INFO:root:FORWARDING request for 'sample.jpg to edge: http://localhost:8002
load-balancer  | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:11:11] "GET /content/sample.jpg HTTP/1.1" 302 -
edge2          | INFO:root:CACHE HIT: sample.jpg
edge2          | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:11:11] "GET /content/sample.jpg HTTP/1.1" 200 -


load-balancer  | INFO:root:FORWARDING request for 'sample.jpg to edge: http://localhost:8002
load-balancer  | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:11:15] "GET /content/sample.jpg HTTP/1.1" 302 -
edge2          | INFO:root:CACHE HIT: sample.jpg
edge2          | INFO:werkzeug:192.168.65.1 - - [05/Jul/2025 18:11:15] "GET /content/sample.jpg HTTP/1.1" 200 -

```
