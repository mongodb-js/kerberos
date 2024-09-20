#!/usr/bin/env bash

set -o errexit  # Exit the script with error if any of the commands fail
set -o xtrace

source ./.drivers-tools/.evergreen/init-node-and-npm-env.sh

# mongodbtoolchain is necessary for building on Windows
export PATH="/opt/mongodbtoolchain/v2/bin:$PATH"

echo "Node Version $(node -v)"

npm run prebuild
echo "Local prebuild successful."
ls prebuilds
