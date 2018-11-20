'use strict';
const kerberos = require('bindings')('kerberos');
const defineOperation = require('../lib/util').defineOperation;
const expect = require('chai').expect;

const testMethod = defineOperation(kerberos._testMethod, [
  { name: 'string', type: 'string' },
  { name: 'shouldError', type: 'boolean', default: false },
  { name: 'callback', type: 'function', required: false }
]);

describe('defineOperation', () => {
  it('should validate parameters', function() {
    expect(() => testMethod(42)).to.throw(/Invalid type for parameter/);
  });

  it('should return a promise if no callback is provided', function() {
    const promise = testMethod('llamas', false);
    expect(promise).to.be.instanceOf(Promise);
  });
});
