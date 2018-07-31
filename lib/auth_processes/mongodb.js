'use strict';
const dns = require('dns');
const kerberos = require('../kerberos');

class MongoAuthProcess {
  constructor(host, port, service_name, options) {
    options = options || {};
    this.host = host;
    this.port = port;

    // Set up service name
    service_name = service_name || 'mongodb';

    // Options
    this.gssapiServiceName = options.gssapiServiceName || service_name;
    this.gssapiServiceRealm = options.gssapiServiceRealm;
    this.gssapiCanonicalizeHostName =
      typeof options.gssapiCanonicalizeHostName === 'boolean'
        ? options.gssapiCanonicalizeHostName
        : false;

    // Set up first transition
    this._transition = this.first_transition(this);

    // Set up target
    this.target = `${service_name}@${host}`;

    // Number of retries
    this.retries = 10;
  }

  init(username, password, callback) {
    const self = this;
    this.username = username;
    this.password = password;

    // Canonicialize host name if needed
    const performGssapiCanonicalizeHostName = function(gssapiCanonicalizeHostName, host, callback) {
      if (!gssapiCanonicalizeHostName) return callback();

      // Attempt to resolve the host name
      dns.resolveCname(host, function(err, r) {
        if (err) return callback(err);

        // Get the first resolve host id
        if (Array.isArray(r) && r.length > 0) {
          self.host = r[0];
        }

        callback();
      });
    };

    // Canonicialize host name if needed
    performGssapiCanonicalizeHostName(this.gssapiCanonicalizeHostName, this.host, function(err) {
      if (err) return callback(err);

      const service = `${self.gssapiServiceName}@${self.host}`;
      kerberos.initializeClient(service, (err, client) => {
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

  first_transition(self) {
    return function(payload, callback) {
      self.client.step('', function(err, response) {
        if (err) return callback(err);

        // Set up the next step
        self._transition = this.second_transition(self);

        // Return the payload
        callback(null, response);
      });
    };
  }

  second_transition(self) {
    return function(payload, callback) {
      self.client.step(payload, function(err, response) {
        if (err && self.retries === 0) return callback(err);

        // Attempt to re-establish a context
        if (err) {
          // Adjust the number of retries
          self.retries = self.retries - 1;

          // Call same step again
          return self.transition(payload, callback);
        }

        // Set up the next step
        self._transition = this.third_transition(self);

        // Return the payload
        callback(null, response || '');
      });
    };
  }

  third_transition(self) {
    return function(payload, callback) {
      // GSS Client Unwrap
      self.client.unwrap(payload, function(err, response) {
        if (err) return callback(err, false);

        // Wrap the response
        self.client.wrap(response, { user: self.username }, (err, wrapped) => {
          if (err) return callback(err, false);

          // Set up the next step
          self._transition = this.fourth_transition(self);

          // Return the payload
          callback(null, wrapped);
        });
      });
    };
  }

  fourth_transition(self) {
    return function(payload, callback) {
      // Set the transition to null
      self._transition = null;

      // Callback with valid authentication
      callback(null, true);
    };
  }
}

// Set the process
module.exports = {
  MongoAuthProcess
};
