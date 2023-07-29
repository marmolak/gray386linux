#/usr/bin/env bash

podman build -t gray386linux:latest -v "${PWD}/..":/build ./
