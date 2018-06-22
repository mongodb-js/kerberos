'use strict';

exports.setUp = function(callback) {
  callback();
};

exports.tearDown = function(callback) {
  callback();
};

exports['Initialize a security Buffer Descriptor'] = function(test) {
  const SecurityBufferDescriptor = require('../../lib/sspi.js').SecurityBufferDescriptor;
  const SecurityBuffer = require('../../lib/sspi.js').SecurityBuffer;

  // Create descriptor with single Buffer
  var securityDescriptor = new SecurityBufferDescriptor(100);
  try {
    // Fail to work due to no valid Security Buffer
    securityDescriptor = new SecurityBufferDescriptor(['hello']);
    test.ok(false);
  } catch (err) {
    // do nothing
  }

  // Should Correctly construct SecurityBuffer
  let buffer = new SecurityBuffer(SecurityBuffer.DATA, 100);
  securityDescriptor = new SecurityBufferDescriptor([buffer]);
  // Should correctly return a buffer
  let result = securityDescriptor.toBuffer();
  test.equal(100, result.length);

  // Should Correctly construct SecurityBuffer
  buffer = new SecurityBuffer(SecurityBuffer.DATA, new Buffer('hello world'));
  securityDescriptor = new SecurityBufferDescriptor([buffer]);
  result = securityDescriptor.toBuffer();
  test.equal('hello world', result.toString());

  // Test passing in more than one Buffer
  buffer = new SecurityBuffer(SecurityBuffer.DATA, new Buffer('hello world'));
  let buffer2 = new SecurityBuffer(SecurityBuffer.STREAM, new Buffer('adam and eve'));
  securityDescriptor = new SecurityBufferDescriptor([buffer, buffer2]);
  result = securityDescriptor.toBuffer();
  test.equal('hello worldadam and eve', result.toString());
  test.done();
};
