FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y \
  software-properties-common cmake libcurl4-openssl-dev rapidjson-dev build-essential && \
  add-apt-repository ppa:gijzelaar/snap7 && apt-get update && apt-get install -y libsnap7-dev && \
  rm -rf /var/lib/apt/lists/*
WORKDIR /plcwatchd
COPY . /plcwatchd
RUN cmake . -DCMAKE_BUILD_TYPE=Release && cmake --build . && make install

FROM ubuntu:22.04
RUN apt-get update && apt-get install -y software-properties-common && \
  add-apt-repository ppa:gijzelaar/snap7 && apt-get update && apt-get install -y libsnap7-1 curl && \
  rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/bin/plcwatchd /usr/local/bin/
ENTRYPOINT ["plcwatchd"]
