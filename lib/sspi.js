'use strict';

// Load the native SSPI classes
const SecurityBuffer = require('./win32/wrappers/security_buffer').SecurityBuffer;
const SecurityBufferDescriptor = require('./win32/wrappers/security_buffer_descriptor')
  .SecurityBufferDescriptor;
const SecurityCredentials = require('./win32/wrappers/security_credentials').SecurityCredentials;
const SecurityContext = require('./win32/wrappers/security_context').SecurityContext;

module.exports = {
  SecurityBuffer,
  SecurityBufferDescriptor,
  SecurityCredentials,
  SecurityContext
};
