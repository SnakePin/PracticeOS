#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
docker run --rm -it --tmpfs "/tmp" -v "$SCRIPT_DIR/..:/work" \
	-p "127.0.0.1:1234:1234" \
	-e DISPLAY=unix$DISPLAY -v "/tmp/.X11-unix:/tmp/.X11-unix" \
	practiceos "$@"
