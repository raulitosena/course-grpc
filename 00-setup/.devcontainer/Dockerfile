FROM ubuntu:latest

# Install necessary packages
RUN apt update && apt install -y cmake build-essential gdb git git-flow autoconf libtool pkg-config valgrind sudo
# Set ubuntu password
RUN echo 'ubuntu:ubuntu' | chpasswd

# Switch to ubuntu user
USER ubuntu

# gRPC build and installation
WORKDIR /home/ubuntu/grpc_build
ENV SOURCES_DIR=/home/ubuntu/grpc_sources
ENV INSTALL_DIR=/home/ubuntu/.local
RUN git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc $SOURCES_DIR
RUN mkdir -p $SOURCES_DIR/cmake/build && cd $SOURCES_DIR/cmake/build && mkdir -p $INSTALL_DIR
RUN cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR $SOURCES_DIR
RUN make -j $(nproc)
RUN make install
RUN rm -rf /home/ubuntu/grpc_sources
RUN rm -rf /home/ubuntu/grpc_build
ENV PATH="$INSTALL_DIR/bin:$PATH"

WORKDIR /home/ubuntu
RUN ln -s /workspaces workspaces
