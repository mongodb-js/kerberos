#!/usr/bin/env bash
set -o errexit  # Exit the script with error if any of the commands fail

# allowed values:
## a nodejs major version (i.e., 16)
## 'latest'
## a full nodejs version, in the format v<major>.<minor>.patch
export NODE_LTS_VERSION=${NODE_LTS_VERSION:-14}
# npm version can be defined in the environment for cases where we need to install
# a version lower than latest to support EOL Node versions.
export NPM_VERSION=${NPM_VERSION:-latest}

source ./.drivers-tools/.evergreen/install-node.sh

npm install --build-from-source
ldd build/*/kerberos.node || true
