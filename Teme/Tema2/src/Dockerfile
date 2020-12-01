FROM python:3.8

COPY requirements.txt /tmp
RUN pip3 install --upgrade pip
RUN pip3 install -r /tmp/requirements.txt

ADD . /api
WORKDIR /api

EXPOSE 5000

ADD start_server.sh /
RUN chmod +x /start_server.sh

CMD ["/start_server.sh"]
