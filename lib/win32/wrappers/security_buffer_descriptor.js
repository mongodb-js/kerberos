'use strict';

const kerberos = require('bindings')('kerberos');
const SecurityBufferDescriptor = kerberos.SecurityBufferDescriptor;

// Export the modified class
module.exports = {
  SecurityBufferDescriptor
};
