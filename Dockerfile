FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive 

RUN apt-get update && \
    apt-get install -y software-properties-common lsb-release wget 

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg > /dev/null && \
    apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"

RUN apt update && \
    apt-get install -y g++ vim cmake git libboost-dev libgdal-dev  libomp-dev libtbb-dev libgtest-dev libnlopt-dev

RUN cmake --version 

RUN git clone https://github.com/google/benchmark.git 
RUN cd benchmark && mkdir build && cd build && cmake .. -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON && make -j && make install

ADD ./ /app/
WORKDIR /app
RUN rm -rf build
RUN cmake -S . -B build  -DCMAKE_BUILD_TYPE=Release && cd build && make -j ORAM