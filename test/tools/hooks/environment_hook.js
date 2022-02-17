'use strict';

const util = require('util');

function beforeAllEnvironmentLog() {
  console.error(
    util.inspect(
      {
        node: process.version,
        KERBEROS_USERNAME:
          typeof process.env.KERBEROS_USERNAME === 'string' ? 'defined' : 'not defined',
        KERBEROS_PASSWORD:
          typeof process.env.KERBEROS_PASSWORD === 'string' ? 'defined' : 'not defined',
        KERBEROS_HOSTNAME:
          typeof process.env.KERBEROS_HOSTNAME === 'string' ? 'defined' : 'not defined',
        KERBEROS_PORT: typeof process.env.KERBEROS_PORT === 'string' ? 'defined' : 'not defined',
        KERBEROS_REALM: typeof process.env.KERBEROS_REALM === 'string' ? 'defined' : 'not defined'
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
