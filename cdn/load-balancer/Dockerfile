FROM python:3.9-slim

WORKDIR /app

COPY . /app

RUN pip install flask

EXPOSE 7000

CMD [ "python", "app.py" ]