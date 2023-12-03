FROM ubuntu:jammy

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
    make gcc nasm binutils python3 \
    bochs-x bochsbios vgabios qemu-system-i386 qemu-system-gui && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Create user
ARG UNAME=builder
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o $UNAME && useradd -m -u $UID -g $GID -o -s /bin/bash $UNAME

# Switch to user
USER $UNAME
WORKDIR /

ENTRYPOINT ["/usr/bin/env"]
