'use strict';

const kerberos = require('./kerberos');

// Get the Kerberos library
module.exports = kerberos;

// Support legacy versions of the mongodb driver which expect this export
module.exports.Kerberos = kerberos;

module.exports.version = require('../package.json').version;

// Set up the auth processes
module.exports.processes = {
  MongoAuthProcess: require('./auth_processes/mongodb').MongoAuthProcess
};
