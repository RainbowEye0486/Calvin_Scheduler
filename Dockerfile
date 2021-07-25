FROM ubuntu:20.04

WORKDIR /devOps

COPY . /devOps

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update  
RUN apt-get install -y build-essential

# cmake install
RUN apt-get install -y cmake

# install gcc compiler
RUN apt install -y manpages-dev
RUN gcc --version
RUN apt-get install make

# install vim
RUN apt-get install -y vim
