#!/bin/bash
docker run --add-host host.docker.internal:host-gateway -v "${PWD}:/root" --privileged -ti agodio/itba-so:2.0