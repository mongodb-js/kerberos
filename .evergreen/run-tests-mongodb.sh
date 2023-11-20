#!/bin/bash
set -e
set -x
cd "${PROJECT_DIRECTORY}"
rm -rf mongodb-test-envs
git clone git@github.com:mongodb-js/devtools-docker-test-envs.git mongodb-test-envs
pushd mongodb-test-envs
git checkout ca4bacd23e6f7ea07618c303b20556e3e4c9c2e6
popd

FAILED=no
docker-compose \
  -f "$PROJECT_DIRECTORY/mongodb-test-envs/docker/kerberos/docker-compose.yaml" \
  -f "$PROJECT_DIRECTORY/.evergreen/mongodb-tests/docker-compose.yaml" \
  --no-ansi \
  up --build --exit-code-from kerberos_jumphost --abort-on-container-exit || FAILED=yes

docker-compose \
  -f "$PROJECT_DIRECTORY/mongodb-test-envs/docker/kerberos/docker-compose.yaml" \
  -f "$PROJECT_DIRECTORY/.evergreen/mongodb-tests/docker-compose.yaml" \
  --no-ansi \
  down -v

if [ $FAILED = yes ]; then
  exit 1
fi
