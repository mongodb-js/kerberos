#! /usr/bin/env bash

# Only set errexit and xtrace if shell is NOT interactive
[[ $- == *i* ]] || set -o xtrace
[[ $- == *i* ]] || set -o errexit

export PROJECT_DIRECTORY="$(pwd)"
export DRIVERS_TOOLS="$PROJECT_DIRECTORY/.drivers-tools"

if [ ! -d "$DRIVERS_TOOLS" ]; then
  # Only clone driver tools if it does not exist
  git clone --depth=1 "https://github.com/mongodb-labs/drivers-evergreen-tools.git" "${DRIVERS_TOOLS}"
fi

echo "installed DRIVERS_TOOLS from commit $(git -C "${DRIVERS_TOOLS}" rev-parse HEAD)"


# Get the current unique version of this checkout
if [ "${is_patch}" = "true" ]; then
    CURRENT_VERSION=$(git describe)-patch-${version_id}
else
    CURRENT_VERSION=latest
fi

# Python has cygwin path problems on Windows. Detect prospective mongo-orchestration home directory
if [ "Windows_NT" = "$OS" ]; then # Magic variable in cygwin
    export PROJECT_DIRECTORY=$(cygpath -m "$PROJECT_DIRECTORY")
fi

cat <<EOT > expansion.yml
CURRENT_VERSION: "$CURRENT_VERSION"
PROJECT_DIRECTORY: "$PROJECT_DIRECTORY"
PREPARE_SHELL: |
    set -o errexit
    set -o xtrace
    export PROJECT_DIRECTORY="$PROJECT_DIRECTORY"
    export PROJECT="${project}"
EOT

# See what we've done
cat expansion.yml
