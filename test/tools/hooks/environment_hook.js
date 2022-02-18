'use strict';

const util = require('util');

function beforeAllEnvironmentLog() {
  console.error(
    util.inspect(
      {
        node: process.version,
        KERBEROS_USERNAME: process.env.KERBEROS_USERNAME,
        KERBEROS_PASSWORD: process.env.KERBEROS_PASSWORD,
        KERBEROS_HOSTNAME: process.env.KERBEROS_HOSTNAME,
        KERBEROS_PORT: process.env.KERBEROS_PORT,
        KERBEROS_REALM: process.env.KERBEROS_REALM
      },
      { colors: true }
    ) + '\n'
  );
}

module.exports = {
  mochaHooks: {
    beforeAll: [beforeAllEnvironmentLog]
  }
};
