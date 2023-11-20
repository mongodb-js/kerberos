#!/bin/bash
set -e
set -x

export PROJECT_DIRECTORY=/app
cd /app
source .evergreen/init-node-and-npm-env.sh

function get_domain_ip() {
  PATTERN="^PING [^ ]+ \(([^)]+)\)"
  OUT="$(ping -c 1 -n $1)"
  [[ $OUT =~ $PATTERN ]]
  echo "${BASH_REMATCH[1]}"
}

function resolve_hosts() {
  MONGODB_1_IP=$(get_domain_ip "mongodb-kerberos-1.example.com")
  MONGODB_2_IP=$(get_domain_ip "mongodb-kerberos-2.example.com")
  MONGODB_3_IP=$(get_domain_ip "mongodb-kerberos-3.examplecrossrealm.com")
  echo "$MONGODB_1_IP mongodb-kerberos-1.example.com" >> /etc/hosts
  echo "$MONGODB_2_IP mongodb-kerberos-2.example.com" >> /etc/hosts
  echo "$MONGODB_3_IP mongodb-kerberos-3.examplecrossrealm.com" >> /etc/hosts
}

function setup_kerberos() {
  printf "Logging in as mongodb.user@EXAMPLE.COM"
  echo 'password' | kinit mongodb.user@EXAMPLE.COM
  klist
}

echo "Waiting for startup..."
sleep 15 # let all services start up
echo "STARTING TESTS..."

#resolve_hosts
setup_kerberos

rm -f node_modules/kerberos && ln -s $PROJECT_DIRECTORY node_modules/kerberos # let the driver find this package
export CONNECTION_STRING="mongodb://mongodb.user%40EXAMPLE.COM@mongodb-kerberos-1.example.com:27017/?authMechanism=GSSAPI"
node -e '(async function() { // \
  require("kerberos"); // if "kerberos" cannot be loaded, the driver error message is unhelpful, so try to load it separately \
  const { MongoClient } = require("mongodb"); // \
  const client = await MongoClient.connect(process.env.CONNECTION_STRING); // \
  await client.db("admin").command({ping:1}); // \
  await client.close(); })();'
.evergreen/run-tests.sh
