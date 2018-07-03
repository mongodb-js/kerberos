'use strict';
const kerberos = require('..');
const request = require('request');
const expect = require('chai').expect;
const SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

const username = process.env.KERBEROS_USERNAME || 'administrator';
// const password = process.env.KERBEROS_PASSWORD || 'Password01';
const realm = process.env.KERBEROS_REALM || 'example.com';
const hostname = process.env.KERBEROS_HOSTNAME || 'hostname.example.com';
const port = process.env.KERBEROS_PORT || '80';

describe('Kerberos', function() {
  it('should authenticate against a kerberos server using GSSAPI', function(done) {
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

  it('should authenticate against a kerberos HTTP endpoint', function(done) {
    const service = `HTTP@${hostname}`;
    const url = `http://${hostname}:${port}/`;

    // send the initial request un-authenticated
    request(url, (err, response) => {
      // expect a 401 response
      expect(response.statusCode).to.equal(401);

      // validate the response supports the Negotiate protocol
      const authenticateHeader = response.headers['www-authenticate'];
      expect(authenticateHeader).to.exist;
      expect(authenticateHeader).to.equal('Negotiate');

      // generate the first Kerberos token
      const mechOID = kerberos.GSS_MECH_OID_KRB5;
      kerberos.initializeClient(service, { mechOID }, (err, client) => {
        expect(err).to.not.exist;
        client.step('', (err, kerberosToken) => {
          // attach the Kerberos token and resend back to the host
          request.get(
            { url, headers: { Authorization: `Negotiate ${kerberosToken}` } },
            (err, response) => {
              // expect a 200 response
              expect(response.statusCode).to.equal(200);

              // validate the headers exist and contain a www-authenticate message
              const authenticateHeader = response.headers['www-authenticate'];
              expect(authenticateHeader).to.exist;
              expect(authenticateHeader).to.startWith('Negotiate');

              // verify the return Kerberos token
              const tokenParts = authenticateHeader.split(' ');
              const serverKerberosToken = tokenParts[tokenParts.length - 1];
              client.step(serverKerberosToken, err => {
                expect(err).to.not.exist;
                expect(client.contextComplete).to.be.true;
                done();
              });
            }
          );
        });
      });
    });
  });
});
