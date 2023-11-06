ARG UBUNTU_VERSION=latest
FROM ubuntu:${UBUNTU_VERSION}

ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y -q && \
    apt-get upgrade -y -q && \
    apt-get install -y -q --no-install-recommends \
        ca-certificates \
        cmake \
        gcc \
        g++ \
        git \
        libeigen3-dev \
        libgtest-dev \
        make \
        software-properties-common \
        wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Creating in and out directories
RUN mkdir /in
RUN mkdir /out

# Setting workdir to /in
WORKDIR /in