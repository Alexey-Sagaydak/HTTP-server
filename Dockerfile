FROM ubuntu:22.04

RUN apt-get update && apt-get install -y libboost-all-dev

COPY ./server/build/http-server /usr/local/bin/http-server

EXPOSE 7777

CMD ["http-server"]
