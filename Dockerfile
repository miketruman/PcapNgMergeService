FROM ubuntu AS builder

WORKDIR /build/
RUN apt-get update -y
RUN apt-get install -y git
RUN git clone https://github.com/seladb/PcapPlusPlus.git
WORKDIR /build/PcapPlusPlus/
RUN ./configure-linux.sh .
RUN apt-get install -y build-essential
RUN apt-get install -y libpcap-dev
RUN make
RUN make install
WORKDIR /build/
RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata
RUN apt-get install -y libboost-all-dev
COPY main.cpp ./
COPY makefile ./
RUN make

FROM scratch
COPY --from=builder /build/a.bin /build/a.bin
EXPOSE 8080
CMD ["/build/a.bin" ]
