'use strict';
const kerberos = require('..');
const expect = require('chai').expect;
const SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

const username = process.env.KERBEROS_USERNAME || 'administrator';
// const password = process.env.KERBEROS_PASSWORD || 'Password01';
const realm = process.env.KERBEROS_REALM || 'example.com';
const hostname = process.env.KERBEROS_HOSTNAME || 'hostname.example.com';
// const port = process.env.KERBEROS_PORT || '80';

describe('GSSAPI', function() {
  it('should correctly authenticate against a kerberos server', function(done) {
    const service = `HTTP@${hostname}`;

    kerberos.initializeClient(service, {}, (err, client) => {
      expect(err).to.not.exist;

      kerberos.initializeServer(service, (err, server) => {
        expect(err).to.not.exist;
        expect(client.contextComplete).to.be.false;
        expect(server.contextComplete).to.be.false;

        client.step('', (err, clientResponse) => {
          expect(err).to.not.exist;
          expect(client.contextComplete).to.be.false;

          server.step(clientResponse, (err, serverResponse) => {
            expect(err).to.not.exist;
            expect(client.contextComplete).to.be.false;

            client.step(serverResponse, err => {
              expect(err).to.not.exist;
              expect(client.contextComplete).to.be.true;

              const expectedUsername = `${username}@${realm.toUpperCase()}`;
              expect(server.username).to.equal(expectedUsername);
              expect(client.username).to.equal(expectedUsername);
              expect(server.targetName).to.not.exist;
              done();
            });
          });
        });
      });
    });
  });
});
