#!/bin/bash
set -o errexit  # Exit the script with error if any of the commands fail

get_version_at_git_rev () {
  local REV=$1
  local VERSION=$(node -r child_process -e "console.log(JSON.parse(child_process.execSync('git show $REV:./package.json', { encoding: 'utf8' })).version);")
  echo $VERSION
}

run_prebuild() {
  if [[ -z $NODE_GITHUB_TOKEN ]];then
    echo "No github token set. Cannot run prebuild."
    exit 1
  else
    echo "Github token detected. Running prebuild."
    npm run prebuild -- -u $NODE_GITHUB_TOKEN
    npm run prebuild-legacy -- -u $NODE_GITHUB_TOKEN
    echo "Prebuild's successfully submitted"
  fi
}

NODE_ARTIFACTS_PATH="${PROJECT_DIRECTORY}/node-artifacts"

if [ "$OS" == "Windows_NT" ]; then
    export NVM_HOME=`cygpath -w "$NODE_ARTIFACTS_PATH/nvm"`
    export NVM_SYMLINK=`cygpath -w "$NODE_ARTIFACTS_PATH/bin"`
    export PATH=`cygpath $NVM_SYMLINK`:`cygpath $NVM_HOME`:$PATH
else
    export PATH="/opt/mongodbtoolchain/v2/bin:$PATH"
    export NVM_DIR="${NODE_ARTIFACTS_PATH}/nvm"
    [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"
fi


VERSION_AT_HEAD=$(get_version_at_git_rev "HEAD")
VERSION_AT_HEAD_1=$(get_version_at_git_rev "HEAD~1")

if [[ ! -z $NODE_FORCE_PUBLISH ]]; then
  echo '$NODE_FORCE_PUBLISH detected'
  echo "Beginning prebuild"
  run_prebuild
elif [[ $VERSION_AT_HEAD != $VERSION_AT_HEAD_1 ]]; then
  echo "Difference is package version ($VERSION_AT_HEAD_1 -> $VERSION_AT_HEAD)"
  echo "Beginning prebuild"
  run_prebuild
else
  echo "No difference is package version ($VERSION_AT_HEAD_1 -> $VERSION_AT_HEAD)"
  echo "Will prebuild without submit"
  npm run prebuild
  echo "Local prebuild successful."
  ls ./prebuilds
fi