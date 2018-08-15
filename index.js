'use strict';

const kerberos = require('./lib/kerberos');

// Get the Kerberos library
module.exports = kerberos;

// Support legacy versions of the mongodb driver which expect this export
module.exports.Kerberos = kerberos;

// Set up the auth processes
module.exports.processes = {
  MongoAuthProcess: require('./lib/auth_processes/mongodb').MongoAuthProcess
};
