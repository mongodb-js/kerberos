#!/usr/bin/env bash

set -o errexit  # Exit the script with error if any of the commands fail
set -o xtrace

source $DRIVERS_TOOLS/.evergreen/init-node-and-npm-env.sh

npm run check:lint
npm test
