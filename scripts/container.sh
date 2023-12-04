#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

DOCKER_USER_ARG="--user $(id -u):$(id -g)"
if [[ $(docker --version) =~ "podman" ]]
then
	DOCKER_USER_ARG="$DOCKER_USER_ARG --userns keep-id"
fi

docker run --rm -it $DOCKER_USER_ARG \
	--tmpfs "/tmp" -v "$SCRIPT_DIR/..:/work" \
	-p "127.0.0.1:1234:1234" \
	-e DISPLAY=unix$DISPLAY -v "/tmp/.X11-unix:/tmp/.X11-unix" \
	practiceos "$@"
