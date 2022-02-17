#! /usr/bin/env bash

set +o xtrace   # Turn off trace
echo "Setting up NVM environment"

export PATH="/opt/mongodbtoolchain/v2/bin:$PATH"
NODE_ARTIFACTS_PATH="${PROJECT_DIRECTORY}/node-artifacts"
export NVM_DIR="${NODE_ARTIFACTS_PATH}/nvm"

if [[ "$OS" == "Windows_NT" ]]; then
    echo "Sourcing nvm shell script (Windows)"
    NVM_HOME=$(cygpath -w "$NVM_DIR")
    export NVM_HOME
    NVM_SYMLINK=$(cygpath -w "$NODE_ARTIFACTS_PATH/bin")
    export NVM_SYMLINK
    NVM_ARTIFACTS_PATH=$(cygpath -w "$NODE_ARTIFACTS_PATH/bin")
    export NVM_ARTIFACTS_PATH
    PATH=$(cygpath $NVM_SYMLINK):$(cygpath $NVM_HOME):$PATH
    export PATH
    echo "updated path on windows PATH=$PATH"
else
    echo "Sourcing nvm shell script"
    [ -s "$NVM_DIR/nvm.sh" ] && source "$NVM_DIR/nvm.sh"
fi

set -o xtrace # Write all commands first to stderr
