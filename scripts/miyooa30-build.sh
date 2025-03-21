#!/bin/bash

docker run --rm -it --mount type=bind,source="$(cygpath -w "$(pwd)")/../src",target=/workspace/src miyooa30-toolchain bash -c "cd /workspace/src && source ~/config.sh && make miyooa30"
docker image prune -f
rm -rf VortexXeGame.tar.gz
tar -czf VortexXeGame.tar.gz -C ../src/dist --transform 's,^,VortexXeGame/,' .