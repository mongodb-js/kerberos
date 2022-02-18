'use strict';

const chai = require('chai');
const expect = chai.expect;
const api = require('../lib/index');

describe('module', function () {
  it('should export version', function () {
    expect(api.version).to.be.a('string');
    expect(api.version).to.match(/\d+\.\d+/);
  });

  it('should export flags and ids', function () {
    [
      api.GSS_C_DELEG_FLAG,
      api.GSS_C_MUTUAL_FLAG,
      api.GSS_C_REPLAY_FLAG,
      api.GSS_C_SEQUENCE_FLAG,
      api.GSS_C_CONF_FLAG,
      api.GSS_C_INTEG_FLAG,
      api.GSS_C_ANON_FLAG,
      api.GSS_C_PROT_READY_FLAG,
      api.GSS_C_TRANS_FLAG,
      api.GSS_C_NO_OID,
      api.GSS_MECH_OID_KRB5,
      api.GSS_MECH_OID_SPNEGO
    ].forEach(flag => expect(flag).to.be.a('number'));
  });

  it('should export functions', function () {
    expect(api.initializeClient).to.be.a('function');
    expect(api.initializeServer).to.be.a('function');
    expect(api.principalDetails).to.be.a('function');
    expect(api.checkPassword).to.be.a('function');
  });

  it('should export Kerberos', () => {
    expect(api.Kerberos).to.be.an('object');
  });
});
