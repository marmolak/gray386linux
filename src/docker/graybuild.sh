#/usr/bin/env bash

podman build --no-cache -t gray386linux:latest -v $PWD/..:/build ./
