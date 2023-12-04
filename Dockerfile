FROM ubuntu:jammy

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
    make gcc g++ binutils python3 \
    bochs-x bochsbios vgabios qemu-system-i386 qemu-system-gui && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# We need NASM 2.16 for DWARF with DW_AT_comp_dir
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y wget ca-certificates && \
    wget https://launchpad.net/ubuntu/+archive/primary/+files/nasm_2.16.01-1_amd64.deb -O /tmp/nasm_amd64.deb && \
    dpkg -i /tmp/nasm_amd64.deb && rm -f /tmp/nasm_amd64.deb && \
    DEBIAN_FRONTEND=noninteractive apt-get --purge autoremove -y ca-certificates wget && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Create user
ARG UNAME=user
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o $UNAME && useradd -m -u $UID -g $GID -o -s /bin/bash $UNAME

# Switch to user
USER $UNAME
WORKDIR /work

ENTRYPOINT ["/usr/bin/env"]
