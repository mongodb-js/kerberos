'use strict';
const dns = require('dns');
const kerberos = require('../kerberos');

class MongoAuthProcess {
  constructor(host, port, serviceName, options) {
    options = options || {};
    this.host = host;
    this.port = port;

    // Set up service name
    serviceName = serviceName || options.gssapiServiceName || 'mongodb';

    // Options
    this.canonicalizeHostName =
      typeof options.gssapiCanonicalizeHostName === 'boolean'
        ? options.gssapiCanonicalizeHostName
        : false;

    this.service =
      process.platform === 'win32' ? `${serviceName}/${host}` : `${serviceName}@${host}`;

    // Set up first transition
    this._transition = firstTransition(this);

    // Number of retries
    this.retries = 10;
  }

  init(username, password, callback) {
    const self = this;
    this.username = username;
    this.password = password;

    // Canonicialize host name if needed
    function performGssapiCanonicalizeHostName(canonicalizeHostName, host, callback) {
      if (!canonicalizeHostName) return callback();

      // Attempt to resolve the host name
      dns.resolveCname(host, (err, r) => {
        if (err) return callback(err);

        // Get the first resolve host id
        if (Array.isArray(r) && r.length > 0) {
          self.host = r[0];
        }

        callback();
      });
    }

    // Canonicialize host name if needed
    performGssapiCanonicalizeHostName(this.canonicalizeHostName, this.host, err => {
      if (err) return callback(err);

      kerberos.initializeClient(self.service, { user: username, password }, (err, client) => {
        if (err) return callback(err, null);

        self.client = client;
        callback(null, client);
      });
    });
  }

  transition(payload, callback) {
    if (this._transition == null) {
      return callback(new Error('Transition finished'));
    }

    this._transition(payload, callback);
  }
}

function firstTransition(auth) {
  return (payload, callback) => {
    auth.client.step('', (err, response) => {
      if (err) return callback(err);

      // Set up the next step
      auth._transition = secondTransition(auth);

      // Return the payload
      callback(null, response);
    });
  };
}

function secondTransition(auth) {
  return (payload, callback) => {
    auth.client.step(payload, (err, response) => {
      if (err && auth.retries === 0) return callback(err);

      // Attempt to re-establish a context
      if (err) {
        // Adjust the number of retries
        auth.retries = auth.retries - 1;

        // Call same step again
        return auth.transition(payload, callback);
      }

      // Set up the next step
      auth._transition = thirdTransition(auth);

      // Return the payload
      callback(null, response || '');
    });
  };
}

function thirdTransition(auth) {
  return (payload, callback) => {
    // GSS Client Unwrap
    auth.client.unwrap(payload, (err, response) => {
      if (err) return callback(err, false);

      // Wrap the response
      auth.client.wrap(response, { user: auth.username }, (err, wrapped) => {
        if (err) return callback(err, false);

        // Set up the next step
        auth._transition = fourthTransition(auth);

        // Return the payload
        callback(null, wrapped);
      });
    });
  };
}

function fourthTransition(auth) {
  return (payload, callback) => {
    // Set the transition to null
    auth._transition = null;

    // Callback with valid authentication
    callback(null, true);
  };
}

// Set the process
module.exports = {
  MongoAuthProcess
};
