'use strict';

const SecurityCredentials = require('../../../build/Release/kerberos').SecurityCredentials;

// Add simple kebros helper
SecurityCredentials.aquire_kerberos = function(username, password, domain, callback) {
  if (typeof password === 'function') {
    callback = password;
    password = null;
  } else if (typeof domain === 'function') {
    callback = domain;
    domain = null;
  }

  // We are going to use the async version
  if (typeof callback === 'function') {
    return SecurityCredentials.acquire('Kerberos', username, password, domain, callback);
  } else {
    return SecurityCredentials.aquireSync('Kerberos', username, password, domain);
  }
};

// Export the modified class
module.exports = {
  SecurityCredentials
};
