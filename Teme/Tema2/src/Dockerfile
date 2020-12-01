FROM python:3.8

COPY requirements.txt /tmp
RUN pip3 install --upgrade pip
RUN pip3 install -r /tmp/requirements.txt
RUN apt-get update && apt-get install -y netcat

ADD . /server
WORKDIR /server

EXPOSE 5000

ADD start_server.sh /
RUN chmod +x /start_server.sh

CMD ["./start_server.sh"]
