'use strict';
const kerberos = require('../lib/index');
const chai = require('chai');
const expect = chai.expect;
const os = require('os');
const { test } = require('mocha');
chai.use(require('chai-string'));

// environment variables
const username = process.env.KERBEROS_USERNAME || 'administrator';
const password = process.env.KERBEROS_PASSWORD || 'Password01';
const realm = process.env.KERBEROS_REALM || 'example.com';
const hostname = process.env.KERBEROS_HOSTNAME || 'hostname.example.com';
const port = process.env.KERBEROS_PORT || '80';

describe('Kerberos', function () {
  before(function () {
    if (os.type() === 'Windows_NT') this.skip();
  });

  it('should lookup principal details on a server', async function () {
    const expected = `HTTP/${hostname}@${realm.toUpperCase()}`;
    const details = await kerberos.principalDetails('HTTP', hostname);
    expect(details).to.equal(expected);
  });

  it('should check a given password against a kerberos server', async function () {
    const service = `HTTP/${hostname}`;

    await kerberos.checkPassword(username, password, service, realm.toUpperCase());

    const error = await kerberos
      .checkPassword(username, 'incorrect-password', service, realm.toUpperCase())
      .catch(e => e);
    expect(error).to.be.instanceOf(Error);
  });

  it('should authenticate against a kerberos server using GSSAPI', async function () {
    const service = `HTTP@${hostname}`;

    const client = await kerberos.initializeClient(service, {});
    const server = await kerberos.initializeServer(service);

    expect(client.contextComplete).to.be.false;
    expect(server.contextComplete).to.be.false;

    const clientResponse = await client.step('');
    expect(client.contextComplete).to.be.false;

    const serverResponse = await server.step(clientResponse);

    expect(client.contextComplete).to.be.false;

    await client.step(serverResponse);
    expect(client.contextComplete).to.be.true;

    const expectedUsername = `${username}@${realm.toUpperCase()}`;
    expect(server.username).to.equal(expectedUsername);
    expect(client.username).to.equal(expectedUsername);
    expect(server.targetName).to.not.exist;
  });

  it('should authenticate against a kerberos HTTP endpoint', async function () {
    const service = `HTTP@${hostname}`;
    const url = `http://${hostname}:${port}/`;

    // send the initial request un-authenticated
    const initialResponse = await fetch(url);
    expect(initialResponse.status).to.equal(401);

    // validate the response supports the Negotiate protocol
    const authenticateHeader = initialResponse.headers.get('www-authenticate');
    expect(authenticateHeader).to.exist;
    expect(authenticateHeader).to.equal('Negotiate');

    // generate the first Kerberos token
    const mechOID = kerberos.GSS_MECH_OID_KRB5;
    const client = await kerberos.initializeClient(service, { mechOID });
    const kerberosToken = await client.step('');

    // attach the Kerberos token and resend back to the host
    const authenticatedResponse = await fetch(url, {
      headers: { Authorization: `Negotiate ${kerberosToken}` }
    });
    expect(authenticatedResponse.status).to.equal(200);

    // validate the headers exist and contain a www-authenticate message
    const responseAuthHeader = authenticatedResponse.headers.get('www-authenticate');
    expect(responseAuthHeader).to.exist;
    expect(responseAuthHeader).to.startWith('Negotiate');

    // verify the return Kerberos token
    const tokenParts = responseAuthHeader.split(' ');
    const serverKerberosToken = tokenParts[tokenParts.length - 1];
    await client.step(serverKerberosToken);
    expect(client.contextComplete).to.be.true;
  });

  describe('Client.wrap()', function () {
    async function establishConext() {
      const service = `HTTP@${hostname}`;
      client = await kerberos.initializeClient(service, {});
      server = await kerberos.initializeServer(service);
      const clientResponse = await client.step('');
      const serverResponse = await server.step(clientResponse);
      await client.step(serverResponse);
      expect(client.contextComplete).to.be.true;
      return { client, server };
    }

    let client;
    let server;

    before(establishConext);
    describe('options.protect', function () {
      context('valid values for `protect`', function () {
        test('no options provided', async function () {
          await client.wrap('challenge');
        });

        test('options provided (protect omitted)', async function () {
          await client.wrap('challenge', {});
        });

        test('protect = false', async function () {
          await client.wrap('challenge', { protect: false });
        });

        test('protect = true', async function () {
          await client.wrap('challenge', { protect: true });
        });
      });

      context('when set to an invalid value', function () {
        it('throws a TypeError', async function () {
          const error = await client.wrap('challenge', { protect: 'non-boolean' }).catch(e => e);
          expect(error)
            .to.be.instanceOf(TypeError)
            .to.match(/options.protect must be a boolean/);
        });
      });
    });
  });

  describe('parameter validation', function () {
    test('initializeClient() throws if service is not a string', async function () {
      expect(await kerberos.initializeClient().catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`service` must be a string/);
    });

    test('initializeServer() throws if service is not a string', async function () {
      expect(await kerberos.initializeServer().catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`service` must be a string/);
    });

    test('principalDetails() throws if service is not a string', async function () {
      expect(await kerberos.principalDetails(3, 'foo').catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`service` must be a string/);
    });

    test('principalDetails() throws if hostname is not a string', async function () {
      expect(await kerberos.principalDetails('foo', 3).catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`hostname` must be a string/);
    });

    test('checkPassword() throws if username is not a string', async function () {
      expect(await kerberos.checkPassword(3, 'password', 'service').catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`username` must be a string/);
    });

    test('checkPassword() throws if password is not a string', async function () {
      expect(await kerberos.checkPassword('username', 3, 'service').catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`password` must be a string/);
    });

    test('checkPassword() throws if service is not a string', async function () {
      expect(await kerberos.checkPassword('username', 'password', 3).catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`service` must be a string/);
    });

    test('KerberosServer.step() throws if challenge is not a string', async function () {
      const service = `HTTP@${hostname}`;

      const server = await kerberos.initializeServer(service);
      expect(await server.step(3).catch(e => e))
        .to.be.instanceOf(TypeError)
        .to.match(/`challenge` must be a string/);
    });

    describe('KerberosClient', function () {
      let client;
      beforeEach(async function () {
        const service = `HTTP@${hostname}`;

        client = await kerberos.initializeClient(service);
      });

      test('KerberosClient.unwrap() throws if challenge is not a string', async function () {
        expect(await client.unwrap(3).catch(e => e))
          .to.be.instanceOf(TypeError)
          .to.match(/`challenge` must be a string/);
      });

      test('KerberosClient.wrap() throws if challenge is not a string', async function () {
        expect(await client.wrap(3).catch(e => e))
          .to.be.instanceOf(TypeError)
          .to.match(/`challenge` must be a string/);
      });

      test('KerberosClient.step() throws if challenge is not a string', async function () {
        expect(await client.step(3).catch(e => e))
          .to.be.instanceOf(TypeError)
          .to.match(/`challenge` must be a string/);
      });
    });
  });
});
