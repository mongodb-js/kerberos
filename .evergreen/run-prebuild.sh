#!/usr/bin/env bash

set -o errexit  # Exit the script with error if any of the commands fail
set -o xtrace

source "${PROJECT_DIRECTORY}/.evergreen/init-nvm.sh"

echo "Node Version $(node -v)"

get_version_at_git_rev () {
  local REV=$1
  local VERSION
  VERSION=$(node -r child_process -e "console.log(JSON.parse(child_process.execSync('git show $REV:./package.json', { encoding: 'utf8' })).version);")
  echo "$VERSION"
}

run_prebuild() {
  set +o xtrace # Don't log the token
  if [[ -z $NODE_GITHUB_TOKEN ]];then
    echo "No github token set. Cannot run prebuild."
    exit 1
  else
    echo "Github token detected. Running prebuild."
    npm run prebuild -- -u "${NODE_GITHUB_TOKEN}"
    echo "Prebuild's successfully submitted"
  fi
  set -o xtrace
}


VERSION_AT_HEAD=$(get_version_at_git_rev "HEAD")
VERSION_AT_HEAD_1=$(get_version_at_git_rev "HEAD~1")

if [[ -n $NODE_FORCE_PUBLISH ]]; then
  echo 'NODE_FORCE_PUBLISH detected'
  echo "Beginning prebuild"
  run_prebuild
elif [[ $VERSION_AT_HEAD != "$VERSION_AT_HEAD_1" ]]; then
  echo "Difference is package version ($VERSION_AT_HEAD_1 -> $VERSION_AT_HEAD)"
  echo "Beginning prebuild"
  run_prebuild
else
  echo "No difference is package version ($VERSION_AT_HEAD_1 -> $VERSION_AT_HEAD)"
  echo "Will prebuild without submit"
  npm run prebuild
  echo "Local prebuild successful."
  ls prebuilds
fi
