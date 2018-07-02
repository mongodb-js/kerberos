'use strict';
const kerberos = require('..');
const expect = require('chai').expect;

const username = process.env.KERBEROS_USERNAME || 'administrator';
// const password = process.env.KERBEROS_PASSWORD || 'Password01';
const realm = process.env.KERBEROS_REALM || 'example.com';
const hostname = process.env.KERBEROS_HOSTNAME || 'hostname.example.com';
// const port = process.env.KERBEROS_PORT || '80';

describe('GSSAPI', function() {
  it('should correctly authenticate against a kerberos server', function(done) {
    const service = `HTTP@${hostname}`;

    console.log('initializing client');
    kerberos.initializeClient(service, {}, (err, client) => {
      expect(err).to.not.exist;

      console.log('initializing server');
      kerberos.initializeServer(service, (err, server) => {
        expect(err).to.not.exist;

        console.log('stepping client');
        client.step('', (err, result) => {
          expect(err).to.not.exist;
          expect(result).to.equal(kerberos.AUTH_GSS_COMPLETE);

          console.log('stepping server');
          server.step(client.response, (err, result) => {
            expect(err).to.not.exist;
            expect(result).to.equal(kerberos.AUTH_GSS_COMPLETE);

            console.log('stepping client again');
            client.step(server.response, (err, result) => {
              expect(err).to.not.exist;
              expect(result).to.equal(kerberos.AUTH_GSS_COMPLETE);

              console.log('checking expectations');
              const expectedUsername = `${username}@${realm.toUpperCase()}`;
              expect(server.username).to.equal(expectedUsername);
              expect(client.username).to.equal(expectedUsername);

              expect(server.targetName).to.not.be.empty;
              done();
            });
          });
        });
      });
    });
  });
});

/*
    rc = kerberos.authGSSClientStep(vc, "")
    assert rc == 0, "authGSSClientStep = %d, expecting 0" % rc

    rs = kerberos.authGSSServerStep(vs, kerberos.authGSSClientResponse(vc))
    assert rs != -1, "authGSSServerStep = %d, not expecting it to be -1" % rs

    rc = kerberos.authGSSClientStep(vc, kerberos.authGSSServerResponse(vs))
    assert rc != -1, "authGSSClientStep = %d, not expecting it to be -1" % rc

    expected_username = "%s@%s" % (username, realm.upper())
    server_user_name = kerberos.authGSSServerUserName(vs)
    assert server_user_name == expected_username, "Invalid server username returned"

    client_user_name = kerberos.authGSSClientUserName(vc)
    assert client_user_name == expected_username, "Invalid client username returned"

    server_target_name = kerberos.authGSSServerTargetName(vs)
    assert server_target_name is None, "Server target name is not None"

    rc = kerberos.authGSSClientClean(vc)
    assert rc == 1, "authGSSClientClean = %d, expecting it to be 0" % rc

    rs = kerberos.authGSSServerClean(vs)
    assert rs == 1, "authGSSServerClean = %d, expecting it to be 0" % rs

*/
