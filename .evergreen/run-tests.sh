#!/usr/bin/env bash

set -o errexit  # Exit the script with error if any of the commands fail
set -o xtrace

export NODE_VERSION="16"
export NVM_EXEC="${PROJECT_DIRECTORY}/bindings/node/node-artifacts/nvm/nvm-exec"

$NVM_EXEC npm run check:lint
$NVM_EXEC npm test
