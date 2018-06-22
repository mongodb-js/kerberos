'use strict';

const SecurityBufferDescriptor = require('../../../build/Release/kerberos')
  .SecurityBufferDescriptor;

// Export the modified class
module.exports = {
  SecurityBufferDescriptor
};
