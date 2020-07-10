#!/bin/bash
# set -o xtrace   # Write all commands first to stderr
set -o errexit  # Exit the script with error if any of the commands fail

NODE_LTS_NAME=${NODE_LTS_NAME:-carbon}
NODE_ARTIFACTS_PATH="${PROJECT_DIRECTORY}/node-artifacts"
NPM_CACHE_DIR="${NODE_ARTIFACTS_PATH}/npm"
NPM_TMP_DIR="${NODE_ARTIFACTS_PATH}/tmp"

NVM_WINDOWS_URL="https://github.com/coreybutler/nvm-windows/releases/download/1.1.7/nvm-noinstall.zip"
NVM_URL="https://raw.githubusercontent.com/creationix/nvm/v0.34.0/install.sh"

# this needs to be explicitly exported for the nvm install below
export NVM_DIR="${NODE_ARTIFACTS_PATH}/nvm"
export XDG_CONFIG_HOME=${NODE_ARTIFACTS_PATH}

# create node artifacts path if needed
mkdir -p ${NVM_DIR}
mkdir -p ${NPM_CACHE_DIR}
mkdir -p "${NPM_TMP_DIR}"

# install Node.js
echo "Installing Node ${NODE_LTS_NAME}"
if [ "$OS" == "Windows_NT" ]; then
  export NVM_HOME=`cygpath -w "$NVM_DIR"`
  export NVM_SYMLINK=`cygpath -w "$NODE_ARTIFACTS_PATH/bin"`
  export PATH=`cygpath $NVM_SYMLINK`:`cygpath $NVM_HOME`:$PATH

  # download and install nvm
  curl -L $NVM_WINDOWS_URL -o nvm.zip
  unzip -d $NVM_DIR nvm.zip
  rm nvm.zip

  chmod 777 $NVM_DIR
  chmod -R a+rx $NVM_DIR

  cat <<EOT > $NVM_DIR/settings.txt
root: $NVM_HOME
path: $NVM_SYMLINK
EOT

  nvm install --no-progress --lts=${NODE_LTS_NAME}
  nvm use --lts=${NODE_LTS_NAME}
else
  curl -o- $NVM_URL | bash
  [ -s "${NVM_DIR}/nvm.sh" ] && \. "${NVM_DIR}/nvm.sh"

  nvm install --no-progress --lts=${NODE_LTS_NAME}
fi

# setup npm cache in a local directory
cat <<EOT > .npmrc
devdir=${NPM_CACHE_DIR}/.node-gyp
init-module=${NPM_CACHE_DIR}/.npm-init.js
cache=${NPM_CACHE_DIR}
tmp=${NPM_TMP_DIR}
registry=https://registry.npmjs.org
EOT

# install node dependencies
npm install