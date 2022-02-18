'use strict';
const kerberos = require('bindings')('kerberos');
const defineOperation = require('../lib/util').defineOperation;
const expect = require('chai').expect;

const testMethod = defineOperation(kerberos._testMethod, [
  { name: 'string', type: 'string' },
  { name: 'shouldError', type: 'boolean', default: false },
  { name: 'optionalString', type: 'string', required: false },
  { name: 'callback', type: 'function', required: false }
]);

describe('defineOperation', () => {
  it('should validate parameters', function () {
    expect(() => testMethod(42)).to.throw(/Invalid type for parameter/);
  });

  it('should validate optional parameters, with valid parameters after', function () {
    expect(() => testMethod('llamas', false, true, () => {})).to.throw(
      /Invalid type for parameter `optionalString`/
    );
  });

  it('should support defaults', function (done) {
    expect(() => testMethod('testing')).to.not.throw();
    testMethod('testing', true, err => {
      expect(err).to.exist;
      done();
    });
  });

  it('should return a promise if no callback is provided', function () {
    const promise = testMethod('llamas', false);
    expect(promise).to.be.instanceOf(Promise);
  });

  it('should use a callback if provided', function (done) {
    testMethod('testing', false, 'optional', (err, result) => {
      expect(err).to.not.exist;
      expect(result).to.equal('optional');

      testMethod('testing', true, 'optional', (err, result) => {
        expect(err).to.exist;
        expect(result).to.not.exist;

        done();
      });
    });
  });
});
