FROM ubuntu:24.04

RUN apt-get update && apt-get install -y libboost-all-dev

COPY ./server/build/libhv-http /usr/local/bin/http-server

WORKDIR /usr/local/bin/

EXPOSE 7777

CMD ["http-server"]
