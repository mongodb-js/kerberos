'use strict';

const kerberos = require('bindings')('kerberos');
const SecurityBuffer = kerberos.SecurityBuffer;

// Add some attributes
SecurityBuffer.VERSION = 0;
SecurityBuffer.EMPTY = 0;
SecurityBuffer.DATA = 1;
SecurityBuffer.TOKEN = 2;
SecurityBuffer.PADDING = 9;
SecurityBuffer.STREAM = 10;

// Export the modified class
module.exports = {
  SecurityBuffer
};
