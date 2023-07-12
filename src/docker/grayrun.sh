#!/usr/bin/env bash

podman run --rm -ti -v $PWD/..:/build localhost/gray386linux:latest /bin/sh
