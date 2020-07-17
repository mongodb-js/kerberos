#!/bin/bash

set -e

IP_ADDRESS=$(hostname -I)
HOSTNAME=$(cat /etc/hostname)
NODE_LTS=${NODE_LTS:-argon}

export KERBEROS_HOSTNAME=$HOSTNAME.$KERBEROS_REALM
export DEBIAN_FRONTEND=noninteractive

echo "Installing all the packages required in this test"
apt-get update
apt-get -y -qq install \
  python curl \
  build-essential libkrb5-dev \
  krb5-user krb5-kdc krb5-admin-server \
  apache2 libapache2-mod-auth-gssapi

echo "Configure the hosts file for Kerberos to work in a container"
cp /etc/hosts ~/hosts.new
sed -i "/.*$HOSTNAME/c\\$IP_ADDRESS\t$KERBEROS_HOSTNAME" ~/hosts.new
cp -f ~/hosts.new /etc/hosts

echo "Setting up Kerberos config file at /etc/krb5.conf"
cat > /etc/krb5.conf << EOL
[libdefaults]
    default_realm = ${KERBEROS_REALM^^}
    dns_lookup_realm = false
    dns_lookup_kdc = false
[realms]
    ${KERBEROS_REALM^^} = {
        kdc = $KERBEROS_HOSTNAME
        admin_server = $KERBEROS_HOSTNAME
    }
[domain_realm]
    .$KERBEROS_REALM = ${KERBEROS_REALM^^}
[logging]
    kdc = FILE:/var/log/krb5kdc.log
    admin_server = FILE:/var/log/kadmin.log
    default = FILE:/var/log/krb5lib.log
EOL

echo "Setting up kerberos ACL configuration at /etc/krb5kdc/kadm5.acl"
mkdir -p /etc/krb5kdc
echo -e "*/*@${KERBEROS_REALM^^}\t*" > /etc/krb5kdc/kadm5.acl

echo "Creating KDC database"
# krb5_newrealm returns non-0 return code as it is running in a container, ignore it for this command only
set +e
printf "$KERBEROS_PASSWORD\n$KERBEROS_PASSWORD" | krb5_newrealm
set -e

echo "Creating principals for tests"
kadmin.local -q "addprinc -pw $KERBEROS_PASSWORD $KERBEROS_USERNAME"

echo "Adding principal for Kerberos auth and creating keytabs"
kadmin.local -q "addprinc -randkey HTTP/$KERBEROS_HOSTNAME"
kadmin.local -q "addprinc -randkey host/$KERBEROS_HOSTNAME@${KERBEROS_REALM^^}"
kadmin.local -q "addprinc -randkey host/${KERBEROS_HOSTNAME^^}@${KERBEROS_REALM^^}"
kadmin.local -q "addprinc -randkey ${KERBEROS_HOSTNAME^^}@${KERBEROS_REALM^^}"

kadmin.local -q "ktadd -k /etc/krb5.keytab host/$KERBEROS_HOSTNAME@${KERBEROS_REALM^^}"
kadmin.local -q "ktadd -k /etc/krb5.keytab host/${KERBEROS_HOSTNAME^^}@${KERBEROS_REALM^^}"
kadmin.local -q "ktadd -k /etc/krb5.keytab ${KERBEROS_HOSTNAME^^}@${KERBEROS_REALM^^}"
kadmin.local -q "ktadd -k /etc/krb5.keytab HTTP/$KERBEROS_HOSTNAME"
chmod 777 /etc/krb5.keytab

echo "Restarting Kerberos KDS service"
service krb5-kdc restart

echo "Add ServerName to Apache config"
grep -q -F "ServerName $KERBEROS_HOSTNAME" /etc/apache2/apache2.conf || echo "ServerName $KERBEROS_HOSTNAME" >> /etc/apache2/apache2.conf

echo "Deleting default virtual host file"
rm /etc/apache2/sites-enabled/000-default.conf
rm /etc/apache2/sites-available/000-default.conf
rm /etc/apache2/sites-available/default-ssl.conf

echo "Create website directory structure and pages"
mkdir -p /var/www/example.com/public_html
chmod -R 755 /var/www
echo "<html><head><title>Title</title></head><body>body mesage</body></html>" > /var/www/example.com/public_html/index.html

echo "Create virtual host files"
cat > /etc/apache2/sites-available/example.com.conf << EOL
<VirtualHost *:$KERBEROS_PORT>
    ServerName $KERBEROS_HOSTNAME
    ServerAlias $KERBEROS_HOSTNAME
    DocumentRoot /var/www/example.com/public_html
    ErrorLog ${APACHE_LOG_DIR}/error.log
    CustomLog ${APACHE_LOG_DIR}/access.log combined
    <Directory "/var/www/example.com/public_html">
        AuthType GSSAPI
        AuthName "GSSAPI Single Sign On Login"
        Require user $KERBEROS_USERNAME@${KERBEROS_REALM^^}
        GssapiCredStore keytab:/etc/krb5.keytab
    </Directory>
</VirtualHost>
EOL

echo "Enabling virtual host site"
a2ensite example.com.conf
service apache2 restart

echo "Getting ticket for Kerberos user"
echo -n "$KERBEROS_PASSWORD" | kinit "$KERBEROS_USERNAME@${KERBEROS_REALM^^}"

echo "Try out the curl connection"
CURL_OUTPUT=$(curl --negotiate -u : "http://$KERBEROS_HOSTNAME")

if [ "$CURL_OUTPUT" != "<html><head><title>Title</title></head><body>body mesage</body></html>" ]; then
    echo -e "ERROR: Did not get success message, cannot continue with actual tests:\nActual Output:\n$CURL_OUTPUT"
    exit 1
else
    echo -e "SUCCESS: Apache site built and set for Kerberos auth\nActual Output:\n$CURL_OUTPUT"
fi

NODE_VERSION=${NODE_VERSION:-14}
NODE_ARTIFACTS_PATH="${HOME}/node-artifacts"
NPM_CACHE_DIR="${NODE_ARTIFACTS_PATH}/npm"
NPM_TMP_DIR="${NODE_ARTIFACTS_PATH}/tmp"

export NVM_DIR="${NODE_ARTIFACTS_PATH}/nvm"

# create node artifacts path if needed
mkdir -p ${NVM_DIR}
mkdir -p ${NPM_CACHE_DIR}
mkdir -p "${NPM_TMP_DIR}"

echo "Installing Node.js"
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.3/install.sh | bash

[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"
nvm install $NODE_VERSION

cat <<EOT > .npmrc
devdir=${NPM_CACHE_DIR}/.node-gyp
init-module=${NPM_CACHE_DIR}/.npm-init.js
cache=${NPM_CACHE_DIR}
tmp=${NPM_TMP_DIR}
registry=https://registry.npmjs.org
EOT

echo "Installing dependencies and running test"
npm install --unsafe-perm
npm test
