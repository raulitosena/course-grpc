FROM ubuntu:latest

# Install necessary packages
RUN apt update && apt install -y cmake build-essential git git-flow autoconf libtool pkg-config sudo

# Set the environment variables for gRPC build
ENV SOURCES_DIR=/tmp/grpc_sources
ENV INSTALL_DIR=/home/ubuntu/.local

# Switch to ubuntu user
USER ubuntu
WORKDIR /tmp/grpc_build

# gRPC build and installation
RUN git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc $SOURCES_DIR
RUN mkdir -p $SOURCES_DIR/cmake/build && cd $SOURCES_DIR/cmake/build && mkdir -p $INSTALL_DIR
RUN cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR $SOURCES_DIR
RUN make -j 4
RUN make install
RUN rm -rf /tmp/grpc_sources
RUN rm -rf /tmp/grpc_build

# Add gRPC binaries to PATH
ENV PATH="$INSTALL_DIR/bin:$PATH"
